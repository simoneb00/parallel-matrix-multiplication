#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <unistd.h>

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
    float *Bt = (float*)malloc(sizeof(float)* A_cols * B_cols);
    transpose(B, Bt, A_cols, B_cols);

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

    write_matrix_to_file("openmp_result.bin", C, A_rows, B_cols);

    free(Bt);
}


void save_result_to_file(double time, int m, int k, int n, int num_threads) {
    if (access("openmp_execution_data.csv", F_OK) == 0) {
            // file exists
            FILE *file = fopen("openmp_execution_data.csv", "a");
            if (file == NULL) {
                perror("Error in opening CSV file");
                exit(1);
            }

            fprintf(file, "%d,%d,%d,%d,%f\n", m, k, n, num_threads, time);
            fclose(file);

        } else {
            // file doesn't exist
            FILE *file = fopen("openmp_execution_data.csv", "w");
            if (file == NULL) {
                perror("Error in opening CSV file");
                exit(1);
            }

            fprintf(file, "m,k,n,num_threads,execution_time\n");
            fprintf(file, "%d,%d,%d,%d,%f\n", m, k, n, num_threads, time);
            fclose(file);
        }
}


int main(int argc, char **argv) {

    if (argc != 5) {
        printf("Usage: ./openmp_computation A_rows A_cols B_rows num_threads\n");
        return 1;
    }

    double dtime = omp_get_wtime();

    float *A;
    float *B;
    float *C;

    int A_rows = atoi(argv[1]);
    int A_cols = atoi(argv[2]);
    int B_rows = A_cols;
    int B_cols = atoi(argv[3]);

    int num_threads = atoi(argv[4]);
    omp_set_num_threads(num_threads);

    read_matrix_from_file("A.bin", &A, A_rows, A_cols);
    read_matrix_from_file("B.bin", &B, B_rows, B_cols);
    read_matrix_from_file("C.bin", &C, A_rows, B_cols);

    multiply(A, B, C, A_rows, A_cols, B_cols);
    dtime = omp_get_wtime() - dtime;

    save_result_to_file(dtime, A_rows, A_cols, B_cols, num_threads);
}