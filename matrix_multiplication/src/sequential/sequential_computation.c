#include <stdio.h>
#include "sequential_computation.h"
#include "../utils/print_matrix/print_matrix.h"


/* Compute C = C + A*B (sequentially) 
*  A must be a m x k matrix
*  B must be a k x n matrix
*  C must be a m x n matrix       
*/
void compute_sequential(float **A, float **B, float **C, int m, int k, int n) {
    printf("Starting sequential computation\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int l = 0; l < k; l++) {
                C[i][j] += A[i][l]*B[l][j]; 
            }
        }
    }

    #ifdef DEBUG
    print_matrix(m, n, C);
    #endif
}