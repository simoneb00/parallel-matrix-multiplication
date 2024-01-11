#include <stdio.h>
#include "matrix_gen/matrix_gen.h"
#include "sequential/sequential_computation.h"

#define ROWS_B 23
#define COLS_B 17
#define ROWS_A 17
#define COLS_A 23

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
    float **A = generate_random_matrix(ROWS_A, COLS_A, 12345);
    writeMatrixToFile("A.bin", A, ROWS_A, COLS_A);
    float **B = generate_random_matrix(ROWS_B, COLS_B, 54321);
    writeMatrixToFile("B.bin", B, ROWS_B, COLS_B);
    //float **C = generate_random_matrix(ROWS, COLS, 14235);
    //writeMatrixToFile("C.bin", C, ROWS, COLS);

    printf("Matrix A:\n");
    print_matrix(ROWS_A, COLS_A, A);
    printf("Matrix B:\n");
    print_matrix(ROWS_B, COLS_B, B);
    //compute_sequential(A, B, C, ROWS, ROWS, ROWS);
}