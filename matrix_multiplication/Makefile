CC = g++
CFLAGS = -Wall -Wextra
SRC_DIR = ./src
MATRIX_GEN_DIR = ${SRC_DIR}/matrix_gen
SEQUENTIAL_COMP_DIR = ${SRC_DIR}/sequential
PARALLEL_COMP_DIR = ${SRC_DIR}/parallel
MATRIX_COMPARISON_DIR = ${SRC_DIR}/sequential/matrix_comparison
UTILS_DIR = ${SRC_DIR}/utils

.PHONY:
	all
	clean
	main

all: sequential

sequential: main matrix_comparison matrix_rw
	@echo
	@echo "Compiling for sequential execution"
	${CC} -c ${CFLAGS} ${SEQUENTIAL_COMP_DIR}/sequential_computation.c -o sequential_computation.o
	${CC} -pg ${CFLAGS} main.o sequential_computation.o matrix_comparison.o matrix_rw.o -o matrix_multiplication
	rm *.o
	@echo "Compilation successful"
	@echo

parallel:
	@echo
	@echo "Compiling for parallel execution"
	mpicc -lm ${CFLAGS} ${PARALLEL_COMP_DIR}/init.c -o parallel_computation

matrix_generation: matrix_rw
	@echo
	@echo "Compiling for matrices generation"
	${CC} ${CFLAGS} -c ${MATRIX_GEN_DIR}/matrix_generation.cpp -o matrix_generation.o
	${CC} -pg ${CFLAGS} matrix_generation.o matrix_rw.o -o matrix_generation
	rm *.o 
	@echo 
	@echo "Compilation successful"

main:
	${CC} ${CFLAGS} -c ${SRC_DIR}/main.c -o main.o 

matrix_comparison:
	${CC} ${CFLAGS} -c ${MATRIX_COMPARISON_DIR}/matrix_comparison.c -o matrix_comparison.o

matrix_rw:
	${CC} ${CFLAGS} -c ${UTILS_DIR}/matrix_file_rw/matrix_rw.c -o matrix_rw.o


clean:
	@echo
	@echo "Cleaning up..."
	rm -f matrix_multiplication
	rm -f parallel_computation