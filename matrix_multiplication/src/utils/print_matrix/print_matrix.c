#include <stdio.h>
#include "print_matrix.h"

void print_matrix(int rows, int cols, float **matrix) {
    for (int i = 0; i < rows; i++) {
        puts("\n");
        for (int j = 0; j < cols; j++) {
            printf("%d ", (int)matrix[i][j]);
        }
    }
    puts("\n");
}