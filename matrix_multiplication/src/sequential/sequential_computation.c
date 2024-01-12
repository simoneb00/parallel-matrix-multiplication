#include <stdio.h>
#include "sequential_computation.h"


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

    //#ifdef DEBUG
    printf("\nResult:\n");
    for (int i = 0; i < m; i++) {
        puts("\n");
        for (int j = 0; j < n; j++) {
            printf("%f ", C[i][j]);
        }
    }
    puts("\n");
    //#endif
}
