#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

void write_matrix_to_file(const char *filename, float *matrix, int rows, int cols) {
    FILE* file = fopen(filename, "wb"); 

    if (file == NULL) {
        perror("Errore nell'apertura del file");
        return;
    }

    fwrite(matrix, sizeof(float), rows * cols, file);

    fclose(file);
}

void read_matrix_from_file(const char *filename, float **matrix, int rows, int cols) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        fprintf(stderr, "Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    *matrix = (float *)malloc(rows * cols * sizeof(float));

    if (*matrix == NULL) {
        fprintf(stderr, "Errore di allocazione per la matrice.\n");
        exit(EXIT_FAILURE);
    }

    fread(*matrix, sizeof(float), rows * cols, file);
    fclose(file);
}


void transpose(float *A, float *B, int rows, int cols) {
    int i,j;
    for(i = 0; i < rows; i++) {
        for(j = 0; j < cols; j++) {
            B[j * rows + i] = A[i * cols + j];
        }
    }
}


void multiply(float *A, float *B, float *C, int A_rows, int A_cols, int B_cols) {   
    double dtime = omp_get_wtime();
    float *Bt = (float*)malloc(sizeof(float)* A_cols * B_cols);
    transpose(B, Bt, A_cols, B_cols);
    dtime = omp_get_wtime() - dtime;
    printf("%f\n", dtime);
    dtime = omp_get_wtime();

    #pragma omp parallel
    {
        int i, j, k;
        #pragma omp for
        for (i = 0; i < A_rows; i++) { 
            for (j = 0; j < B_cols; j++) {
                double dot  = 0;
                for (k = 0; k < A_cols; k++) {
                    dot += A[i * A_cols + k] * Bt[j * A_cols + k];
                } 
                C[i * B_cols + j] += dot;            
            }

        }

    }

    dtime = omp_get_wtime() - dtime;
    printf("Multiplication time: %f\n", dtime);


    write_matrix_to_file("openmp_result.bin", C, A_rows, B_cols);

    free(Bt);
}


int main(int argc, char **argv) {
    float *A;
    float *B;
    float *C;

    int A_rows = 2500;
    int A_cols = 5000;
    int B_rows = 5000;
    int B_cols = 2500;

    read_matrix_from_file("A.bin", &A, A_rows, A_cols);
    read_matrix_from_file("B.bin", &B, B_rows, B_cols);
    read_matrix_from_file("C.bin", &C, A_rows, B_cols);

    double dtime = omp_get_wtime();
    multiply(A, B, C, A_rows, A_cols, B_cols);
    dtime = omp_get_wtime() - dtime;
    printf("Execution time: %f\n", dtime);

}