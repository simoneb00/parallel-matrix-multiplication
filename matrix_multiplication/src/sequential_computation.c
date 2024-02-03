#include <stdio.h>
#include "matrix_gen/matrix_gen.h"
#include "sequential/sequential_computation.h"
#include "sequential/matrix_comparison/matrix_comparison.h"
#include "utils/print_matrix/print_matrix.h"
#include "utils/matrix_file_rw/matrix_rw.h"
#include "utils/compute_max_error/compute_max_error.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void save_result_to_file(double time, int m, int k, int n) {
    if (access("sequential_execution_data.csv", F_OK) == 0) {
            // file exists
            FILE *file = fopen("sequential_execution_data.csv", "a");
            if (file == NULL) {
                perror("Error in opening CSV file");
                exit(1);
            }

            fprintf(file, "%d,%d,%d,%f\n", m, k, n, time);
            fclose(file);

        } else {
            // file doesn't exist
            FILE *file = fopen("sequential_execution_data.csv", "w");
            if (file == NULL) {
                perror("Error in opening CSV file");
                exit(1);
            }

            fprintf(file, "m,k,n,execution_time\n");
            fprintf(file, "%d,%d,%d,%f\n", m, k, n, time);
            fclose(file);
        }
}


int main(int argc, char *argv[]) {

    clock_t start_time = clock();

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


    C = compute_sequential(A, B, C, A_rows, A_cols, B_cols);

    clock_t end_time = clock();

    write_matrix_to_file("sequential_result.bin", C, A_rows, B_cols);

    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Sequential execution time: %.4f s\n", elapsed_time);

    save_result_to_file(elapsed_time, A_rows, A_cols, B_cols);
    
    return 0;
}