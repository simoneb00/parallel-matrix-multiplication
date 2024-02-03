#include <stdio.h>
#include "../matrix_gen/matrix_gen.h"
#include "sequential_computation/sequential_computation.h"
#include "matrix_comparison/matrix_comparison.h"
#include "../utils/print_matrix/print_matrix.h"
#include "../utils/matrix_file_rw/matrix_rw.h"
#include "../utils/compute_max_error/compute_max_error.h"
#include <stdlib.h>
#include <time.h>
#include "matrix_mult.h"

void sequential_computation(int A_rows, int A_cols, int B_cols) {
    
    int B_rows = A_cols;
    
    float *A = read_matrix_from_file("A.bin", A_rows, A_cols);
    float *B = read_matrix_from_file("B.bin", B_rows, B_cols);
    float *C = read_matrix_from_file("C.bin", A_rows, B_cols);

    
    clock_t start_time = clock();

    C = compute_sequential(A, B, C, A_rows, A_cols, B_cols);

    clock_t end_time = clock();

    write_matrix_to_file("sequential_result.bin", C, A_rows, B_cols);

    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Sequential execution time: %.4f s\n", elapsed_time);
}