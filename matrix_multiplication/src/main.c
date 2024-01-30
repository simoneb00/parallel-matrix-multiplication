#include <stdio.h>
#include "matrix_gen/matrix_gen.h"
#include "sequential/sequential_computation.h"
#include "sequential/matrix_comparison/matrix_comparison.h"
#include "utils/print_matrix/print_matrix.h"
#include "utils/matrix_file_rw/matrix_rw.h"
#include "utils/compute_max_error/compute_max_error.h"
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: ./matrix_multiplication m k n\n");
        return 1;
    }

    int A_rows = atoi(argv[1]);
    int A_cols = atoi(argv[2]);
    int B_rows = A_cols;
    int B_cols = atoi(argv[3]); 
    
    float *A = read_matrix_from_file("A.bin", A_rows, A_cols);
    float *B = read_matrix_from_file("B.bin", B_rows, B_cols);
    float *C = read_matrix_from_file("C.bin", A_rows, B_cols);

    
    clock_t start_time = clock();

    C = compute_sequential(A, B, C, A_rows, A_cols, B_cols);

    clock_t end_time = clock();

    write_matrix_to_file("sequential_result.bin", C, A_rows, B_cols);

    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Tempo di esecuzione: %.4f secondi\n", elapsed_time);
    

    float *sequential_result = read_matrix_from_file("sequential_result.bin", A_rows, B_cols);

    float *parallel_result = read_matrix_from_file("mpi_result.bin", A_rows, B_cols);

    float *comparison = compare_matrices(parallel_result, sequential_result, A_rows, B_cols);

    float *original_C = read_matrix_from_file("C.bin", A_rows, B_cols);

    int error = 0;

    for (int i = 0; i < A_rows; i++) {
        for (int j = 0; j < B_cols; j++) {
            if (comparison[i * B_cols + j] != 0)
                error = 1;
        }
    }

    if (error) 
        printf("There's an error\n");
    else
        printf("There are no errors\n");


    for (int i = 0; i < 100; i++) {
        printf("%f ", parallel_result[i]);
    }
    puts("\n");

    for (int i = 0; i < 100; i++) {
        printf("%f ", sequential_result[i]);
    }
    puts("\n");

    float max_error = compute_max_error(comparison, A_rows, B_cols);
    printf("Max error: %f\n", max_error);

    return 0;
}