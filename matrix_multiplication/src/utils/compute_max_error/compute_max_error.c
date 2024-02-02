#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float compute_max_error(float *matrix, int rows, int cols, int *x, int *y) {
    float sum_errors = 0.0;
    float max_error = 0.0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            sum_errors += fabs(matrix[i * cols + j]);
            if (fabs(matrix[i * cols + j]) > max_error) {
                max_error = fabs(matrix[i * cols + j]);
                *x = i;
                *y = j;
            }
        }
    }

    printf("Mean error: %.10f\n", sum_errors / (rows * cols));
    printf("Max error at coordinates (%d, %d):\n", *x, *y);

    return max_error;
}