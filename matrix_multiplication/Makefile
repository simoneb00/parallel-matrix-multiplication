CC = g++
CFLAGS = -Wall -Wextra
SRC_DIR = ./src
MATRIX_GEN_DIR = ${SRC_DIR}/matrix_gen
SEQUENTIAL_COMP_DIR = ${SRC_DIR}/sequential
PARALLEL_COMP_DIR = ${SRC_DIR}/parallel

.PHONY:
	all
	clean
	main

all: sequential

sequential: main matrix_generation
	@echo
	@echo "Compiling for sequential execution"
	${CC} -c ${CFLAGS} ${SEQUENTIAL_COMP_DIR}/sequential_computation.c -o sequential_computation.o
	${CC} -pg ${CFLAGS} main.o matrix_generation.o sequential_computation.o -o matrix_multiplication
	rm *.o
	@echo "Compilation successful"
	@echo

parallel:
	@echo
	@echo "Compiling for parallel execution"
	mpicc -lm ${CFLAGS} ${PARALLEL_COMP_DIR}/init.c -o parallel_computation

matrix_generation: 
	${CC} ${CFLAGS} -c ${MATRIX_GEN_DIR}/matrix_generation.cpp -o matrix_generation.o

main:
	${CC} ${CFLAGS} -c ${SRC_DIR}/main.c -o main.o 

clean:
	@echo
	@echo "Cleaning up..."
	rm -f matrix_multiplication