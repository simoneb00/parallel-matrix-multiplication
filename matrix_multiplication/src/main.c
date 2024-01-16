#include <stdio.h>
#include "matrix_gen/matrix_gen.h"
#include "sequential/sequential_computation.h"
#include "sequential/matrix_comparison/matrix_comparison.h"
#include "utils/print_matrix/print_matrix.h"
#include "utils/matrix_file_rw/matrix_rw.h"
#include "utils/compute_max_error/compute_max_error.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: ./matrix_multiplication m k n\n");
        return 1;
    }

    int A_rows = atoi(argv[1]);
    int A_cols = atoi(argv[2]);
    int B_rows = A_cols;
    int B_cols = atoi(argv[3]); 
    
    float **A;
    read_matrix_from_file("A.bin", &A, A_rows, A_cols);
    float **B;
    read_matrix_from_file("B.bin", &B, B_rows, B_cols);
    float **C;
    read_matrix_from_file("C.bin", &C, A_rows, B_cols);
    
    //compute_sequential(A, B, C, A_rows, A_cols, B_cols);
    

    float **res_matrix;
    read_matrix_from_file("parallel_result.bin", &res_matrix, A_rows, B_cols);

    float **comparison = compare_matrices(C, res_matrix, A_rows, B_cols);
    float max_error = compute_max_error(comparison, A_rows, B_cols);
    printf("Max error: %f\n", max_error);

    return 0;
}