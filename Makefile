CC=gcc
MPICC=mpicc
CFLAGS=-O2

all: serial parallel_omp parallel_mpi

serial: src/main_serial.c src/utils.h
	$(CC) $(CFLAGS) -o serial src/main_serial.c

parallel_omp: src/main_parallel.c src/utils.h
	$(CC) $(CFLAGS) -fopenmp -o parallel_omp src/main_parallel.c

parallel_mpi: src/main_parallel_mpi.c src/utils.h
	$(MPICC) $(CFLAGS) -o parallel_mpi src/main_parallel_mpi.c

clean:
	rm -f serial parallel_omp parallel_mpi log*.csv
