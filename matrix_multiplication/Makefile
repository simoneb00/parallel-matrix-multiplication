CC = g++
CFLAGS = -Wall -Wextra
SRC_DIR = ./src
MATRIX_GEN_DIR = ${SRC_DIR}/matrix_gen
SEQUENTIAL_COMP_DIR = ${SRC_DIR}/sequential
PARALLEL_COMP_DIR = ${SRC_DIR}/parallel
MATRIX_COMPARISON_DIR = ${SRC_DIR}/sequential/matrix_comparison
UTILS_DIR = ${SRC_DIR}/utils
OPENMP_DIR = ${SRC_DIR}/parallel/OpenMP

.PHONY:
	all
	clean
	main

all: sequential mpi openmp matrix_comparison matrix_generation
	rm *.o

sequential: matrix_utils
	@echo
	@echo "Compiling for sequential execution"
	${CC} -c -O3 ${CFLAGS} ${SEQUENTIAL_COMP_DIR}/sequential_computation.c -o sequential_computation.o
	${CC} ${CFLAGS} -c ${SRC_DIR}/sequential_computation.c -o sequential.o 
	${CC} ${CFLAGS} sequential.o sequential_computation.o matrix_rw.o print_matrix.o -o sequential_computation
	@echo "Compilation successful"
	@echo

mpi:
	@echo
	@echo "Compiling for MPI execution"
	mpicc ${CFLAGS} -O3 ${PARALLEL_COMP_DIR}/init.c -o mpi_computation -lm

openmp:
	@echo
	@echo "Compiling for OpenMP execution"
	${CC} ${CFLAGS} -O3 -fopenmp ${OPENMP_DIR}/matrix_mult.c -o openmp_computation
	@echo 
	@echo "Compilation successful"


matrix_generation: matrix_utils
	@echo
	@echo "Compiling for matrices generation"
	${CC} ${CFLAGS} -c ${MATRIX_GEN_DIR}/matrix_generation.cpp -o matrix_generation.o
	${CC} ${CFLAGS} matrix_generation.o matrix_rw.o -o matrix_generation
	@echo 
	@echo "Compilation successful"
	

matrix_comparison: matrix_utils max_error
	@echo
	@echo "Compiling for results comparison"
	${CC} ${CFLAGS} -c ${MATRIX_COMPARISON_DIR}/matrix_comparison.c -o matrix_comparison.o
	${CC} ${CFLAGS} matrix_comparison.o matrix_rw.o compute_max_error.o -o comparison

matrix_utils:
	${CC} ${CFLAGS} -c ${UTILS_DIR}/matrix_file_rw/matrix_rw.c -o matrix_rw.o
	${CC} ${CFLAGS} -c ${UTILS_DIR}/print_matrix/print_matrix.c -o print_matrix.o

max_error:
	${CC} ${CFLAGS} -c ${UTILS_DIR}/compute_max_error/compute_max_error.c -o compute_max_error.o

clean:
	@echo
	@echo "Cleaning up..."
	rm -f sequential_computation
	rm -f mpi_computation
	rm -f openmp_computation
	rm -f matrix_generation
	rm -f comparison
	rm -f A.bin
	rm -f B.bin
	rm -f C.bin
	rm -f mpi_result.bin
	rm -f openmp_result.bin
	rm -f sequential_result.bin