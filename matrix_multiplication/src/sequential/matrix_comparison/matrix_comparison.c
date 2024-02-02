#include <stdio.h>
#include <stdlib.h>
#include "matrix_comparison.h"
#include "../../utils/matrix_file_rw/matrix_rw.h"
#include "../../utils/compute_max_error/compute_max_error.h"

float *compare_matrices(float *matrix1, float *matrix2, int rows, int cols) {

    float *result = (float *)malloc(rows * cols * sizeof(float));
    if (result == NULL) {
        fprintf(stderr, "Error in allocating the rows of the resulting matrix.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i * cols + j] = matrix1[i * cols + j] - matrix2[i * cols + j];
        }
    }

    return result;
}

void count_errors(float *comparison, int rows, int cols, int *x, int *y) {
    int count = 0;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (comparison[i * cols + j] != 0) {
                count++;
                //printf("There's an error at coordinates (%d, %d)\n", i, j);
            }
        }
    }
    puts("");

    printf("There are %d errors\n", count);

    float max_error = compute_max_error(comparison, rows, cols, x, y);
    printf("Max error: %f\n", max_error);
}


int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: ./comparison rows cols\n");
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    float *sequential_result = read_matrix_from_file("sequential_result.bin", rows, cols);
    float *mpi_result = read_matrix_from_file("mpi_result.bin", rows, cols);
    float *openmp_result = read_matrix_from_file("openmp_result.bin", rows, cols);

    int x, y;

    puts("");
    printf("Comparing sequential result to mpi result\n");
    float *mpi_comparison = compare_matrices(sequential_result, mpi_result, rows, cols);
    count_errors(mpi_comparison, rows, cols, &x, &y);

    printf("%f  -  %f\n", sequential_result[x * cols + y], mpi_result[x * cols + y]);

    puts("");
    printf("Comparing sequential result to openmp result\n");
    float *openmp_comparison = compare_matrices(sequential_result, openmp_result, rows, cols);
    count_errors(openmp_comparison, rows, cols, &x, &y);

    printf("%f  -  %f\n", sequential_result[x * cols + y], openmp_result[x * cols + y]);
    

    free(sequential_result);
    free(mpi_result);
    free(openmp_result);
    free(mpi_comparison);
    free(openmp_comparison);

}