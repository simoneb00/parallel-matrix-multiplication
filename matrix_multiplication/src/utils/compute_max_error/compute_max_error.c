#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float compute_max_error(float *matrix, int rows, int cols) {
    float max_error = 0.0;
    int count = 0;
    float *errors = (float *)malloc(rows*cols*sizeof(float));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fabs(matrix[i * cols + j]) > max_error) {
                max_error = fabs(matrix[i * cols + j]);
                errors[count] = max_error;
                count++;
            }
        }
    }

    printf("Number of errors: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("%f ", errors[i]);
    }
    puts("");

    return max_error;
}