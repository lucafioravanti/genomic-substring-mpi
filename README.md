# Substring Matching with MPI
This project performs substring matching on large genomic sequences using both serial and parallel implementations in C with MPI.

## How to Build
```bash
make
```

## How to Run
### Serial
```bash
./serial data/test_dna.txt ACGTA
```

### Parallel (e.g., with 4 processes)
```bash
mpiexec -n 4 ./parallel data/test_dna.txt ACGTA
```
