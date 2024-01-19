#include <stdio.h>
#include <stdlib.h>
#include "matrix_comparison.h"

float **compare_matrices(float **matrix1, float **matrix2, int rows, int cols) {

    float **result = (float **)malloc(rows * sizeof(float *));
    if (result == NULL) {
        fprintf(stderr, "Error in allocating the rows of the resulting matrix.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; ++i) {
        result[i] = (float *)malloc(cols * sizeof(float));
        if (result[i] == NULL) {
            fprintf(stderr, "Error in allocating the columns of the resulting matrix.\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i][j] = matrix1[i][j] - matrix2[i][j];
        }
    }

    return result;
}