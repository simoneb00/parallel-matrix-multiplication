#include "matrix_gen.h"
#include <random>
#include <iostream>
#include <fstream>
#include "../utils/matrix_file_rw/matrix_rw.h"

using namespace std;


float **generate_random_matrix(int rows, int cols, int seed, int zero_matrix)
{
    if (!zero_matrix) {
        printf("Generating a matrix with %d rows and %d cols, starting from seed %d\n", rows, cols, seed);
        float** matrix = new float*[rows];
        for (int i = 0; i < rows; ++i) {
            matrix[i] = new float[cols];
        }

        mt19937 gen(seed);                              // mersenne twister.
        uniform_int_distribution<> dist(0, 10);     // distribute results between 0.0 and 10.0 inclusive.

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                matrix[i][j] = (float)dist(gen);
            }
        }

        printf("Matrix successfully generated\n");

        return matrix;
    } else {
        printf("Generating a zero matrix with %d rows and %d cols\n", rows, cols);
        float** matrix = new float*[rows];
        for (int i = 0; i < rows; ++i) {
            matrix[i] = new float[cols];
        }

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                matrix[i][j] = 0;
            }
        }

        printf("Matrix successfully generated\n");

        return matrix;
    }
}

void deallocate_matrix(float **matrix, int rows) {
    for (int i = 0; i < rows; ++i) {
            delete[] matrix[i];
        }
        delete[] matrix;
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: ./matrix_generation m k n\n");
        return 1;
    }

    int A_rows = atoi(argv[1]);
    int A_cols = atoi(argv[2]);
    int B_rows = A_cols;
    int B_cols = atoi(argv[3]); 
    
    float **A = generate_random_matrix(A_rows, A_cols, 12345, 0);
    writeMatrixToFile("A.bin", A, A_rows, A_cols);
    deallocate_matrix(A, A_rows);
    float **B = generate_random_matrix(B_rows, B_cols, 54321, 0);
    writeMatrixToFile("B.bin", B, B_rows, B_cols);
    deallocate_matrix(B, B_rows);
    float **C = generate_random_matrix(A_rows, B_cols, 14235, 1);
    writeMatrixToFile("C.bin", C, A_rows, B_cols);
    deallocate_matrix(C, A_rows);
    
    return 0;
}