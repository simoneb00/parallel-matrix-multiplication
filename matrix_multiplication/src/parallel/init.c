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
int compute_submatrix_dimension(int matrix_rows, int matrix_cols, int block_rows, int block_cols, 
            int proc_rows, int proc_cols, int my_rank, int *num_rows, int *num_cols) {

    int my_rank_coord_x, my_rank_coord_y;

    my_rank_coord_x = my_rank / proc_cols;
    my_rank_coord_y = my_rank % proc_cols;     

    /* COLS */
    int tot_num_blocks = ceil((float)matrix_cols / (float)block_cols); 

    int tot_blocks_assigned = tot_num_blocks / proc_cols;

    if (tot_num_blocks % proc_cols != 0 && my_rank_coord_y < tot_num_blocks % proc_cols) {
            tot_blocks_assigned++;
    } 

    /* assign last block portion */

    int tot_nums_assigned_x = tot_blocks_assigned * block_cols;

    int last_process_x = tot_num_blocks % proc_cols == 0 ? proc_cols -1 : (tot_num_blocks % proc_cols) -1;
    if (my_rank_coord_y == last_process_x && matrix_cols % block_cols != 0) {
        tot_nums_assigned_x -= (block_cols - (matrix_cols % block_cols));
    }

    /* ROWS */ 
    tot_num_blocks = ceil((float)matrix_rows / (float)block_rows);

    tot_blocks_assigned = tot_num_blocks / proc_rows; 

    if (tot_num_blocks % proc_rows != 0 && my_rank_coord_x < tot_num_blocks % proc_rows) {
            tot_blocks_assigned++;
    } 

    /* assign last block portion */

    int tot_nums_assigned_y = tot_blocks_assigned * block_rows; 

    int last_process_y = ((tot_num_blocks % proc_rows) == 0) ? (proc_rows -1) : ((tot_num_blocks % proc_rows) -1);  
    if (my_rank_coord_x == last_process_y && matrix_rows % block_rows != 0) {
        tot_nums_assigned_y -= (block_rows - (matrix_rows % block_rows));
    }

    *num_rows = tot_nums_assigned_y;
    *num_cols = tot_nums_assigned_x;

    #ifdef DEBUG
    printf("[Process %d] Assigned %d rows\n", my_rank, tot_nums_assigned_y);
    printf("[Process %d] Assigned %d cols\n", my_rank, tot_nums_assigned_x);
    printf("[Process %d] Assigned %d elements\n", my_rank, tot_nums_assigned_y * tot_nums_assigned_x);
    puts("");
    #endif

    return tot_nums_assigned_x * tot_nums_assigned_y;

}

/// @brief This function divides the matrices A between all the processes, according to a block-cyclic distribution.
/// @param filename Filename of the matrix binary file 
/// @param matrix Float array that will contain the submatrix
/// @param rows Number of rows of the matrix
/// @param cols Number of columns of the matrix
/// @param block_rows Number of rows of the block
/// @param block_cols Number of columns of the block
/// @param proc_rows Number of rows of the process grid
/// @param proc_cols Number of columns of the process grid
/// @param num_rows Number of rows of the original matrix assigned to the process
/// @param num_cols Number of columns of the original matrix assigned to the process
/// @param comm MPI communicator
float *block_cyclic_distribution(char *filename, float *matrix, int rows, int cols, 
        int block_rows, int block_cols, int proc_rows, int proc_cols, int *num_rows, int *num_cols, MPI_Comm comm) {

    int rank;

    int matrix_size;
    
    MPI_Datatype filetype;
    MPI_File file;

    MPI_Status status;

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

    psizes[0] = proc_rows; /* no. of processes in vertical dimension of process grid */
    psizes[1] = proc_cols; /* no. of processes in horizontal dimension of process grid */

    MPI_Type_create_darray(proc_rows * proc_cols, rank, 2, gsizes, distribs, dargs, psizes, MPI_ORDER_C, MPI_FLOAT, &filetype);
    MPI_Type_commit(&filetype);

    MPI_File_open(comm, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);

    if (file == MPI_FILE_NULL) {
        fprintf(stderr, "Error in opening file.\n");
        MPI_Abort(comm, 1);
    }

    MPI_File_set_view(file, 0, MPI_FLOAT, filetype, "native", MPI_INFO_NULL);

    
    matrix_size = compute_submatrix_dimension(rows, cols, block_rows, block_cols, proc_rows, proc_cols, rank, num_rows, num_cols);

    matrix = (float*) calloc(matrix_size, sizeof(float));
    if (matrix == NULL) {
        fprintf(stderr, "memory allocation failed");
        return NULL;
    }
    MPI_File_read_all(file, matrix, matrix_size,
            MPI_FLOAT, &status);

    MPI_File_close(&file);

    #ifdef DEBUG
    printf("\n======\ni = %d\nBlock received : ", rank);
    for (int i = 0; i < matrix_size; i++) {
        printf("%d ", (int)matrix[i]);
    }
    puts("\n");
    fflush(stdout);
    #endif

    return matrix;

}


/// @brief This function assigns the rows of B to the processes, according to a block-cyclic distribution.
/// @param filename Filename of the binary file containing B
/// @param matrix  Float array that will contain the submatrix
/// @param matrix_rows Rows of B
/// @param matrix_cols Columns of B
/// @param block_rows Rows of the block
/// @param proc_rows Rows of the process grid
/// @param num_rows Number of rows that the current process should get 
/// @param comm MPI communicator
float *divide_rows(char *filename, float *matrix, int matrix_rows, int matrix_cols, 
                    int block_rows, int proc_rows, int num_rows, MPI_Comm comm) {

    int rank;

    int matrix_size;
    
    MPI_Datatype filetype;
    MPI_File file;

    MPI_Status status;

    MPI_Comm_rank(comm, &rank);

    int rank_norm = rank % proc_rows;

    int gsizes[2], distribs[2], dargs[2], psizes[2];

    gsizes[0] = matrix_rows; /* rows of the original matrix */
    gsizes[1] = matrix_cols; /* columns of the original matrix */

    /* block cyclic distribution */
    distribs[0] = MPI_DISTRIBUTE_CYCLIC;
    distribs[1] = MPI_DISTRIBUTE_CYCLIC;

    dargs[0] = block_rows; /* rows of the block */
    dargs[1] = matrix_cols; /* columns of the block */

    psizes[0] = proc_rows; /* no. of processes in vertical dimension of process grid */
    psizes[1] = 1; /* no. of processes in horizontal dimension of process grid (1, because every process in the same row gets the same rows of the original matrix) */

    MPI_Type_create_darray(proc_rows, rank_norm, 2, gsizes, distribs, dargs, psizes, MPI_ORDER_C, MPI_FLOAT, &filetype);
    MPI_Type_commit(&filetype);

    MPI_File_open(comm, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);

    if (file == MPI_FILE_NULL) {
        fprintf(stderr, "Error in opening file.\n");
        MPI_Abort(comm, 1);
    }

    MPI_File_set_view(file, 0, MPI_FLOAT, filetype, "native", MPI_INFO_NULL);
    
    matrix_size = num_rows*matrix_cols;

    matrix = (float*) calloc(matrix_size, sizeof(float));
    if (matrix == NULL) {
        fprintf(stderr, "memory allocation failed");
        return NULL;
    }
    MPI_File_read_all(file, matrix, matrix_size,
            MPI_FLOAT, &status);

    MPI_File_close(&file);

    #ifdef DEBUG
    printf("\n======\ni = %d\nBlock received : ", rank);
    for (int i = 0; i < matrix_size; i++) {
        printf("%d ", (int)matrix[i]);
    }
    puts("\n");
    fflush(stdout);
    #endif

    return matrix;
}


/// @brief This function computes the matrix multiplication between A and B, and saves the result in the array C_temp.
float *multiplyMatrices(float *A, int A_rows, int A_cols, float *B, int B_rows, int B_cols, float *C_temp, int rank) {
    
    if (A_cols != B_rows) {
        fprintf(stderr, "The number of columns in A must be equal to the number of rows in B.\n");
        return NULL;
    }



    C_temp = (float *)calloc(A_rows * B_cols, sizeof(float));
    if (C_temp == NULL) {
        fprintf(stderr, "C_temp allocation failed");
        return NULL;
    }

    for (int i = 0; i < A_rows; ++i) {
        for (int j = 0; j < B_cols; ++j) {
            for (int k = 0; k < A_cols; ++k) {
                C_temp[i * B_cols + j] += A[i * A_cols + k] * B[k * B_cols + j];
            }
        }
    }

    return C_temp;
}


void *send_matrix_to_root(float *matrix, int num_rows, int num_cols, int rank, int proc_rows, int proc_cols, MPI_Comm comm) {
    
    /*
    *   E.g.: 
            Process grid:
            0 1 2 3 4
            5 6 7 8 9

            the processes 0, 1, 2, 3, 4 will send their C_temp to process 0 (1/5 = 2/5 = 3/5 = 4/5 = 0)
            the processes 5, 6, 7, 8, 9 will send their C_temp to process 1 (6/5 = 7/5 = 8/5 = 9/5 = 1)
            the processes 0 and 5 are the receivers
    */

    int row_index = rank / proc_cols;           // this is the index of the row of the process grid in which the current process is located
    int root_process = row_index * proc_cols;   // this is the index of the first process in the same row in which the current process is located

    /* There will be a group for each row in the process grid (so, proc_rows groups).
    *  We're gonna create a new communicator for each group, having color equals to the row index.
    */
    MPI_Comm group_comm;
    MPI_Comm_split(comm, row_index, rank, &group_comm);    // the processes on the same row will be assigned to the same group

    // this array will contain the element-wise sum of all the C_temp matrices belonging to the processes in the same group
    float *result = (float *)calloc(num_rows * num_cols, sizeof(float));  
    if (result == NULL) {
        fprintf(stderr, "result allocation failed");
        return NULL;
    }

    MPI_Reduce(matrix, result, num_rows*num_cols, MPI_FLOAT, MPI_SUM, 0, group_comm);

    MPI_Barrier(group_comm);    // sync barrier

    #ifdef DEBUG
    int group_rank;
    MPI_Comm_rank(group_comm, &group_rank);

    if (group_rank == 0)  {
        printf("\n[Process %d] Printing the resulting matrix C_temp (%dx%d)\n", rank, num_rows, num_cols);
        for (int i = 0; i < num_rows; i++) {
            puts("");
            for (int j = 0; j < num_cols; j++) {
                printf("%d ", (int)result[i*num_cols + j]);
            }
        }
        puts("");
        fflush(stdout);
    }
    #endif

    return result;
}


void write_result_to_file(char *filename, float *result, int result_len, int rank, int matrix_rows, int matrix_cols, int block_rows, int proc_rows, int proc_cols, MPI_Comm comm) {
    
    printf("[Process %d] Trying to write result (length %d) on the resulting matrix (%dx%d) file\n", rank, result_len, matrix_rows, matrix_cols);

    MPI_Datatype filetype;
    MPI_File file;

    MPI_Status status;

    int rank_norm = rank / proc_cols;
    printf("[Process %d] rank_norm: %d\n", rank, rank_norm);

    int gsizes[2], distribs[2], dargs[2], psizes[2];

    gsizes[0] = matrix_rows; /* rows of the original matrix */
    gsizes[1] = matrix_cols; /* columns of the original matrix */

    /* block cyclic distribution */
    distribs[0] = MPI_DISTRIBUTE_CYCLIC;
    distribs[1] = MPI_DISTRIBUTE_CYCLIC;

    dargs[0] = block_rows; /* rows of the block */
    dargs[1] = matrix_cols; /* columns of the block */

    psizes[0] = proc_rows; /* no. of processes in vertical dimension of process grid */
    psizes[1] = 1; /* no. of processes in horizontal dimension of process grid (1, because every process in the same row gets the same rows of the original matrix) */

    MPI_Type_create_darray(proc_rows, rank_norm, 2, gsizes, distribs, dargs, psizes, MPI_ORDER_C, MPI_FLOAT, &filetype);
    MPI_Type_commit(&filetype);
    
    MPI_File_open(comm, filename, MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &file);

    if (file == MPI_FILE_NULL) {
        fprintf(stderr, "Error in opening file.\n");
        MPI_Abort(comm, 1);
    }

    MPI_File_set_view(file, 0, MPI_FLOAT, filetype, "native", MPI_INFO_NULL);
    
    MPI_File_write_all(file, result, result_len, MPI_FLOAT, &status);

    MPI_File_close(&file);
}



int main(int argc, char** argv) {

    int A_rows = 17;
    int A_cols = 23;
    int B_rows = 23;
    int B_cols = 17;
    int result_rows = A_rows;
    int result_cols = B_cols;

    int proc_rows = 2;
    int proc_cols = 5;

    int block_rows = 3;
    int block_cols = 3;

    int n_proc, my_rank;
    MPI_Comm comm;

    MPI_Init(&argc, &argv);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);

    MPI_Comm_size(comm, &n_proc);
    MPI_Comm_rank(comm, &my_rank);

    int num_rows;
    int num_cols;
    float *A = block_cyclic_distribution("A.bin", A, A_rows, A_cols, block_rows, block_cols, proc_rows, proc_cols, &num_rows, &num_cols, comm);

    #ifdef DEBUG
    printf("[Process %d] Got %d elements\n", my_rank, num_rows * num_cols);
    #endif

    float *B = divide_rows("B.bin", B, B_rows, B_cols, block_rows, proc_cols, num_cols, comm);    

    float *C_temp = multiplyMatrices(A, num_rows, num_cols, B, num_cols, B_cols, C_temp, my_rank);


    /* every process sends its C_temp matrix to the first process (say root process) in the same row, in the process grid */
    float *result = send_matrix_to_root(C_temp, num_rows, B_cols, my_rank, proc_rows, proc_cols, comm);

    #ifdef DEBUG
    printf("\n[Process %d] Printing the matrix A (%dx%d)\n", my_rank, num_rows, num_cols);
    for (int i = 0; i < num_rows; i++) {
        puts("");
        for (int j = 0; j < num_cols; j++) {
            printf("%d ", (int)A[i*num_cols + j]);
        }
    }
    puts("");
    fflush(stdout);

    printf("\n[Process %d] Printing the matrix B (%dx%d)\n", my_rank, num_cols, B_cols);
    for (int i = 0; i < num_cols; i++) {
        puts("");
        for (int j = 0; j < B_cols; j++) {
            printf("%d ", (int)B[i*B_cols + j]);
        }
    }
    puts("");
    fflush(stdout);

    printf("\n[Process %d] Printing the matrix C (%dx%d)\n", my_rank, num_rows, B_cols);
    for (int i = 0; i < num_rows; i++) {
        puts("");
        for (int j = 0; j < B_cols; j++) {
            printf("%d ", (int)C_temp[i*B_cols + j]);
        }
    }
    puts("");
    fflush(stdout);
    #endif

    MPI_Barrier(comm);

    MPI_Comm root_comm;
    MPI_Comm_split(comm, my_rank % proc_cols, my_rank, &root_comm);

    int root_rank;
    MPI_Comm_rank(root_comm, &root_rank);

    /* Write result on file */
    if (my_rank % proc_cols == 0) {
        write_result_to_file("parallel_result.bin", result, num_rows * B_cols, my_rank, result_rows, result_cols, block_rows, proc_rows, proc_cols, root_comm);
    }
    
    free(A);
    free(B);
    free(C_temp);

    MPI_Finalize();
}
 