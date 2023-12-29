#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/// @brief This function computes how many values should be assigned to every process: 
///        it determines the max number of blocks that can be assigned to a process, by computing how many blocks
///        there are on the horizontal and vertical "axis" (i.e., rows and cols dimensions) and how these must be 
///        distributed in the process grid, according to a block cyclic distribution.
///        Note that the output of this function is an upper limit: most of the processes need less space.
/// @param matrix_rows Number of rows of the matrix to be distributed
/// @param matrix_cols Number of columns of the matrix to be distributed
/// @param block_rows Number of rows of the block
/// @param block_cols Number of columns of the block
/// @param proc_rows Number of rows of the process grid
/// @param proc_cols Number of columns of the process grid
/// @return Number of floats the process should allocate to receive its portion of the matrix
int compute_submatrix_dimension(int matrix_rows, int matrix_cols, int block_rows, int block_cols, int proc_rows, int proc_cols) {

    /* compute how many blocks there are on the vertical "axis" and how they're distributed among processes */
    float blocks_on_vert = matrix_rows / block_rows;
    int blocks_assigned_on_vert = ceil(blocks_on_vert / proc_rows);

    /* compute how many blocks there are on the horizontal "axis" and how they're distributed among processes */
    float blocks_on_hor = matrix_cols / block_cols;
    int blocks_assigned_on_hor = ceil(blocks_on_hor / proc_cols);

    float total_blocks_assigned = blocks_assigned_on_hor * blocks_assigned_on_vert;

    return total_blocks_assigned * block_rows * block_cols;

}

/// @brief This function divides the matrices A, B and C between n_proc processes, according to a block-cyclic distribution.
/// @param A m x k matrix
/// @param B k x n matrix
/// @param C m x n matrix
/// @param n_proc number of processes to divide the matrices between
/// @param m matrix dimension
/// @param k matrix dimension
/// @param n matrix dimension
void block_cyclic_distribution(char *filename, float *matrix, int rows, int cols, 
        int block_rows, int block_cols, int P, int Q, MPI_Comm comm) {

    int rank;
    int size;

    int matrix_size;
    
    MPI_Datatype filetype;
    MPI_File file;

    MPI_Status status;

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    /**
     * Reading from file.
     */
    int gsizes[2], distribs[2], dargs[2], psizes[2];

    gsizes[0] = rows; /* x dimension of the original matrix */
    gsizes[1] = cols; /* y dimension of the original matrix */

    /* block cyclic distribution */
    distribs[0] = MPI_DISTRIBUTE_CYCLIC;
    distribs[1] = MPI_DISTRIBUTE_CYCLIC;

    dargs[0] = block_rows; /* x dimension of the block */
    dargs[1] = block_cols; /* y dimension of the block */

    psizes[0] = P; /* no. of processes in vertical dimension of process grid */
    psizes[1] = Q; /* no. of processes in horizontal dimension of process grid */

    MPI_Type_create_darray(P * Q, rank, 2, gsizes, distribs, dargs, psizes,
            MPI_ORDER_FORTRAN, MPI_FLOAT, &filetype);
    MPI_Type_commit(&filetype);

    MPI_File_open(comm, filename,
            MPI_MODE_RDONLY,
            MPI_INFO_NULL, &file);

    if (file == MPI_FILE_NULL) {
        fprintf(stderr, "Error in opening file.\n");
        MPI_Abort(comm, 1);
    }

    MPI_File_set_view(file, 0, MPI_FLOAT, filetype, "native",
            MPI_INFO_NULL);

    
    matrix_size = compute_submatrix_dimension(rows, cols, block_rows, block_cols, P, Q);

    matrix = (float*) malloc(matrix_size * sizeof(float));
    MPI_File_read_all(file, matrix, matrix_size,
            MPI_FLOAT, &status);

    MPI_File_close(&file);

    printf("\n======\ni = %d\nBlock received : ", rank);
    for (int i = 0; i < matrix_size; i++) {
        printf("%f ", matrix[i]);
    }
    puts("\n");

}

int main(int argc, char** argv) {

    int n_proc, my_rank;
    MPI_Comm comm;

    MPI_Init(&argc, &argv);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);

    MPI_Comm_size(comm, &n_proc);
    MPI_Comm_rank(comm, &my_rank);

    float *matrix;
    block_cyclic_distribution("A.bin", matrix, 12, 12, 3, 3, 3, 2, comm);    

    // Finalize the MPI environment.
    MPI_Finalize();
}
