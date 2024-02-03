#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "matrix_comparison.h"
#include "../../utils/matrix_file_rw/matrix_rw.h"
#include "../../utils/compute_max_error/compute_max_error.h"


void save_result_to_file_mpi(int m, int k, int n, int proc_rows, int proc_cols, int block_rows, int block_cols, float GFlops, float max_rel_diff, float speedup) {
    if (access("mpi_performance.csv", F_OK) == 0) {
            // file exists
            FILE *file = fopen("mpi_performance.csv", "a");
            if (file == NULL) {
                perror("Error in opening CSV file");
                exit(1);
            }

            fprintf(file, "%d,%d,%d,%d,%d,%d,%d,%f,%e,%f\n", m, k, n, proc_rows, proc_cols, block_rows, block_cols, GFlops, max_rel_diff, speedup);
            fclose(file);

        } else {
            // file doesn't exist
            FILE *file = fopen("mpi_performance.csv", "w");
            if (file == NULL) {
                perror("Error in opening CSV file");
                exit(1);
            }

            fprintf(file, "m,k,n,proc_rows,proc_cols,block_rows,block_cols,GFlops,max_rel_diff,speedup\n");
            fprintf(file, "%d,%d,%d,%d,%d,%d,%d,%f,%e,%f\n", m, k, n, proc_rows, proc_cols, block_rows, block_cols, GFlops, max_rel_diff, speedup);
            fclose(file);
        }
}


void save_result_to_file_openmp(int m, int k, int n, int num_threads, float GFlops, float max_rel_diff, float speedup) {
    if (access("openmp_performance.csv", F_OK) == 0) {
            // file exists
            FILE *file = fopen("openmp_performance.csv", "a");
            if (file == NULL) {
                perror("Error in opening CSV file");
                exit(1);
            }

            fprintf(file, "%d,%d,%d,%d,%f,%e,%f\n", m, k, n, num_threads, GFlops, max_rel_diff, speedup);
            fclose(file);

        } else {
            // file doesn't exist
            FILE *file = fopen("openmp_performance.csv", "w");
            if (file == NULL) {
                perror("Error in opening CSV file");
                exit(1);
            }

            fprintf(file, "m,k,n,num_threads,GFlops,max_rel_diff,speedup\n");
            fprintf(file, "%d,%d,%d,%d,%f,%e,%f\n", m, k, n, num_threads, GFlops, max_rel_diff, speedup);
            fclose(file);
        }
}


float *compare_matrices(float *matrix1, float *matrix2, int rows, int cols) {

    float *result = (float *)malloc(rows * cols * sizeof(float));
    if (result == NULL) {
        fprintf(stderr, "Error in allocating the rows of the resulting matrix.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i * cols + j] = matrix1[i * cols + j] - matrix2[i * cols + j];
        }
    }

    return result;
}

void count_errors(float *comparison, int rows, int cols, int *x, int *y) {
    int count = 0;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (comparison[i * cols + j] != 0) {
                count++;
                //printf("There's an error at coordinates (%d, %d)\n", i, j);
            }
        }
    }
    puts("");

    printf("There are %d errors\n", count);

    float max_error = compute_max_error(comparison, rows, cols, x, y);
    printf("Max error: %f\n", max_error);
}


float find_max_value(const float *matrix, int rows, int cols) {
    float max_value = matrix[0];  // Supponiamo che il primo elemento sia il massimo

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float current_value = matrix[i * cols + j];
            if (current_value > max_value) {
                max_value = current_value;
            }
        }
    }

    return max_value;
}


/* Max diff is defined as the max value in the matrix comparison divided by the max value in both matrices seq_result and par_result  */
float compute_max_rel_diff(float *seq_result, float *par_result, float *comparison, int rows, int cols) {
    float max_value_comparison = find_max_value(comparison, rows, cols);
    float max_value_seq = find_max_value(seq_result, rows, cols);
    float max_value_par = find_max_value(par_result, rows, cols);

    float max_value = max_value_seq > max_value_par ? max_value_seq : max_value_par;

    return max_value_comparison / max_value;
}

float compute_GFlops(int m, int k, int n, int time) {
    return (2*m*n*k/time) * pow(10.0, -9);
}

float compute_speedup(float parallel_time, float sequential_time) {
    return sequential_time / parallel_time;
}


int main(int argc, char *argv[]) {

    printf("%d\n", argc);

    if (argc != 12) {
        printf("Usage: ./comparison m k n proc_rows proc_cols block_rows block_cols num_threads sequential_time parallel_time mpi/openmp\n");
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[3]);

    int k = atoi(argv[2]);
    int proc_rows = atoi(argv[4]);
    int proc_cols = atoi(argv[5]);
    int block_rows = atoi(argv[6]);
    int block_cols = atoi(argv[7]);
    int num_threads = atoi(argv[8]);

    float sequential_time = atof(argv[9]);
    float parallel_time = atof(argv[10]);

    char *type = argv[11];


    float *sequential_result = read_matrix_from_file("sequential_result.bin", rows, cols);


    if (strcmp(type, "mpi")) {
        puts("");
        printf("Comparing sequential result to mpi result\n");

        float *mpi_result = read_matrix_from_file("mpi_result.bin", rows, cols);
        float *mpi_comparison = compare_matrices(sequential_result, mpi_result, rows, cols);

        float max_rel_diff_mpi = compute_max_rel_diff(sequential_result, mpi_result, mpi_comparison, rows, cols);
        float GFlops = compute_GFlops(rows, k, cols, parallel_time);
        float speedup = compute_speedup(parallel_time, sequential_time);

        save_result_to_file_mpi(rows, k, cols, proc_rows, proc_cols, block_rows, block_cols, GFlops, max_rel_diff_mpi, speedup);

        free(mpi_result);
        free(mpi_comparison);

        return 0;
    }

    puts("");
    printf("Comparing sequential result to openmp result\n");

    float *openmp_result = read_matrix_from_file("openmp_result.bin", rows, cols);
    float *openmp_comparison = compare_matrices(sequential_result, openmp_result, rows, cols);

    float max_rel_diff_openmp = compute_max_rel_diff(sequential_result, openmp_result, openmp_comparison, rows, cols);
    float GFlops = compute_GFlops(rows, k, cols, parallel_time);
    float speedup = compute_speedup(parallel_time, sequential_time);

    save_result_to_file_openmp(rows, k, cols, num_threads, GFlops, max_rel_diff_openmp, speedup);

    free(sequential_result);
    free(openmp_result);
    free(openmp_comparison);

}