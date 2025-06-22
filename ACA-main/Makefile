CC=mpicc
CFLAGS=-O2

all: serial parallel

serial: src/main_serial.c
	$(CC) $(CFLAGS) -o serial src/main_serial.c

parallel: src/main_parallel.c
	$(CC) $(CFLAGS) -o parallel src/main_parallel.c

clean:
	rm -f serial parallel
