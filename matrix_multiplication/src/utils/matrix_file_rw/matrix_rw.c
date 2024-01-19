#include <stdlib.h>
#include <stdio.h>
#include "matrix_rw.h"

void read_matrix_from_file(const char *filename, float ***matrix, int rows, int cols) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }


    *matrix = (float **)malloc(rows * sizeof(float *));
    if (*matrix == NULL) {
        perror("Error allocating memory for matrix");
        return;
    }

    (*matrix)[0] = (float *)malloc(rows * cols * sizeof(float));
    if ((*matrix)[0] == NULL) {
        perror("Error allocating memory for matrix data");
        free(*matrix);
        return;
    }

    for (int i = 1; i < rows; i++) {
        (*matrix)[i] = (*matrix)[0] + i * cols;
    }

    fread((*matrix)[0], sizeof(float), rows * cols, file);

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