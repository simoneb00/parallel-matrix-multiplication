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
int compute_submatrix_dimension(int matrix_rows, int matrix_cols, int block_rows, int block_cols, int proc_rows, int proc_cols, int my_rank, int size) {

    int my_rank_coord_x, my_rank_coord_y;

    my_rank_coord_x = my_rank / proc_cols;
    my_rank_coord_y = my_rank % proc_cols; 

    //printf("[Process %d] my coordinates are (%d, %d)\n", my_rank, my_rank_coord_x, my_rank_coord_y);
    

    /* COLS */
    int tot_num_blocks = ceil((float)matrix_cols / (float)block_cols); // 12 / 3 = 4

    int tot_blocks_assigned = tot_num_blocks / proc_cols; // 4 / 2 = 2

    if (tot_num_blocks % proc_cols != 0 && my_rank_coord_y < tot_num_blocks % proc_cols) {
            tot_blocks_assigned++;
    } 

    /* assign last block portion */

    int tot_nums_assigned_x = tot_blocks_assigned * block_cols; // 2 * 3 = 6

    int last_process_x = tot_num_blocks % proc_cols == 0 ? proc_cols -1 : (tot_num_blocks % proc_cols) -1;
    if (my_rank_coord_y == last_process_x && matrix_cols % block_cols != 0) {
        tot_nums_assigned_x -= (block_cols - (matrix_cols % block_cols));
    }

    /* ROWS */ 
    // my_rank_coord_x = 1 
    tot_num_blocks = ceil((float)matrix_rows / (float)block_rows);    // 25 / 3 = 9

    tot_blocks_assigned = tot_num_blocks / proc_rows;   // 9 / 3 = 3

    if (tot_num_blocks % proc_rows != 0 && my_rank_coord_x < tot_num_blocks % proc_rows) {
            tot_blocks_assigned++;
    } 

    /* assign last block portion */

    int tot_nums_assigned_y = tot_blocks_assigned * block_rows; // 3 * 3 = 9

    int last_process_y = ((tot_num_blocks % proc_rows) == 0) ? (proc_rows -1) : ((tot_num_blocks % proc_rows) -1);  // 2
    if (my_rank_coord_x == last_process_y && matrix_rows % block_rows != 0) {
        tot_nums_assigned_y -= (block_rows - (matrix_rows % block_rows));
    }

    printf("[Process %d] Assigned %d rows\n", my_rank, tot_nums_assigned_y);
    printf("[Process %d] Assigned %d cols\n", my_rank, tot_nums_assigned_x);
    printf("[Process %d] Assigned %d elements\n", my_rank, tot_nums_assigned_y * tot_nums_assigned_x);
    puts("");

    return tot_nums_assigned_x * tot_nums_assigned_y;

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
            MPI_ORDER_C, MPI_FLOAT, &filetype);
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

    
    matrix_size = compute_submatrix_dimension(rows, cols, block_rows, block_cols, P, Q, rank, size);

    matrix = (float*) malloc(matrix_size * sizeof(float));
    MPI_File_read_all(file, matrix, matrix_size,
            MPI_FLOAT, &status);

    MPI_File_close(&file);

    /** 
     * Transform the 1D array to a 2D array
     * i.e.: get how many blocks are assigned on this process on the hor axis and on the vert axis,
     * and compute how many rows and columns are in the process' submatrix.
     */

    

    /* Extract blocks from array */
    MPI_Datatype subarray_type; 

    printf("\n======\ni = %d\nBlock received : ", rank);
    for (int i = 0; i < matrix_size; i++) {
        printf("%d ", (int)matrix[i]);
    }
    puts("\n");
    fflush(stdout);

}

int main(int argc, char** argv) {

    int n_proc, my_rank;
    MPI_Comm comm;

    MPI_Init(&argc, &argv);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);

    MPI_Comm_size(comm, &n_proc);
    MPI_Comm_rank(comm, &my_rank);

    float *matrix;
    block_cyclic_distribution("A.bin", matrix, 25, 19, 3, 2, 2, 4, comm);    

    MPI_Finalize();
}
