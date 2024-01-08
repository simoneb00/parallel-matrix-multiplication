#include <stdio.h>
#include "matrix_gen/matrix_gen.h"
#include "sequential/sequential_computation.h"

#define ROWS 12
#define COLS 12

void print_matrix(int rows, int cols, float **matrix) {
    for (int i = 0; i < rows; i++) {
        puts("\n");
        for (int j = 0; j < cols; j++) {
            printf("%d ", (int)matrix[i][j]);
        }
    }
    puts("\n");
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

int main(int argc, char *argv[]) {
    float **A = generate_random_matrix(ROWS, COLS, 12345);
    writeMatrixToFile("A.bin", A, ROWS, COLS);
    float **B = generate_random_matrix(ROWS, COLS, 54321);
    writeMatrixToFile("B.bin", B, ROWS, COLS);
    float **C = generate_random_matrix(ROWS, COLS, 14235);
    writeMatrixToFile("C.bin", C, ROWS, COLS);

    print_matrix(ROWS, COLS, B);
    //compute_sequential(A, B, C, ROWS, ROWS, ROWS);
}