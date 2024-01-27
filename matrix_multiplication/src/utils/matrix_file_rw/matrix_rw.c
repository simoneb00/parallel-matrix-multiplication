#include <stdlib.h>
#include <stdio.h>
#include "matrix_rw.h"

/*
void read_matrix_from_file(const char *filename, float **matrix, int rows, int cols) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }


    *matrix = (float *)malloc(rows * cols * sizeof(float *));
    if (*matrix == NULL) {
        perror("Error allocating memory for matrix");
        return;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i * cols + j] = matrix[i][j];
        }
    }

    fclose(file);
}

void writeMatrixToFile(const char* filename, float** matrix, int rows, int cols) {
    FILE* file = fopen(filename, "wb"); 

    if (file == NULL) {
        perror("Errore nell'apertura del file");
        return;
    }

    for (int i = 0; i < rows; ++i) {
        fwrite(matrix[i], sizeof(float), cols, file);
    }

    fclose(file);
}
*/

void write_matrix_to_file(const char *filename, float *matrix, int rows, int cols) {
    FILE *file;

    if ((file = fopen(filename, "wb")) == NULL) {
        perror("Error in opening file");
        exit(EXIT_FAILURE);
    }

    fwrite(matrix, sizeof(float), rows * cols, file);
    fclose(file);
}

float* read_matrix_from_file(const char *filename, int rows, int cols) {
    FILE *file;
    float *matrix;

    if ((file = fopen(filename, "rb")) == NULL) {
        perror("Error in opening file");
        exit(EXIT_FAILURE);
    }

    matrix = (float *)malloc((rows) * (cols) * sizeof(int));
    fread(matrix, sizeof(float), (rows) * (cols), file);
    fclose(file);

    return matrix;
}