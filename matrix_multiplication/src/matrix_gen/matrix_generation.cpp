#include "matrix_gen.h"
#include <random>
#include <iostream>
#include <fstream>

using namespace std;


float **generate_random_matrix(int rows, int cols, int seed)
{
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

    /*
    // Deallocazione della matrice dalla heap
    for (int i = 0; i < rows; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
    */

    printf("Matrix successfully generated\n");

    return matrix;
}