# Substring Matching with MPI

This project performs substring matching on large genomic sequences using both serial and parallel implementations in C using MPI.

## How to Use

### Compile the Project

To compile both the serial and parallel versions, simply run:

```bash
make
```

This will compile the following:
- `src/main_serial.c` → creates the executable `serial`
- `src/main_parallel.c` → creates the executable `parallel`

---

### Run the Serial Version

Once compiled, you can run the serial version like this:

```bash
./serial data/test_dna.txt ACGTA
```

This command will:
- Read the DNA sequence from `data/test_dna.txt`
- Search for the substring `"ACGTA"`
- Print the positions where the pattern occurs

---

### Run the Parallel Version (MPI)

To run the parallel version, use `mpiexec` and specify the number of processes (e.g., 4):

```bash
mpiexec -n 4 ./parallel data/test_dna.txt ACGTA
```

This command will:
- Launch 4 MPI processes
- Each process searches a portion of the sequence
- Each process prints the matching positions it finds

---

### Note

Make sure MPI is installed on your system.  