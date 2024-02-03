#include "matrix_gen.h"
#include <random>
#include <iostream>
#include <fstream>
#include "../utils/matrix_file_rw/matrix_rw.h"

using namespace std;


float* generate_random_matrix(int rows, int cols, int seed, int zero_matrix) {
    if (!zero_matrix) {
        std::cout << "Generating a matrix with " << rows << " rows and " << cols << " cols, starting from seed " << seed << std::endl;
        float* matrix = new float[rows * cols];

        std::mt19937 gen(seed);                                         // mersenne twister.
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);         // distribute results between 0.0 and 1.0 inclusive.

        for (int i = 0; i < rows * cols; ++i) {
            matrix[i] = dist(gen);
        }

        std::cout << "Matrix successfully generated" << std::endl;

        return matrix;
    } else {
        std::cout << "Generating a zero matrix with " << rows << " rows and " << cols << " cols" << std::endl;
        float* matrix = new float[rows * cols];

        for (int i = 0; i < rows * cols; ++i) {
            matrix[i] = 0;
        }

        std::cout << "Matrix successfully generated" << std::endl;

        return matrix;
    }
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
    
    float *A = generate_random_matrix(A_rows, A_cols, 342234, 0);
    write_matrix_to_file("A.bin", A, A_rows, A_cols);
    delete[] A;

    float *B = generate_random_matrix(B_rows, B_cols, 19423, 0);
    write_matrix_to_file("B.bin", B, B_rows, B_cols);
    delete[] B;

    float *C = generate_random_matrix(A_rows, B_cols, 14235, 0);
    write_matrix_to_file("C.bin", C, A_rows, B_cols);
    delete[] C;
    
    return 0;
}