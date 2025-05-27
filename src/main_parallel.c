#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_SEQ_LEN 1000000
#define MAX_PATTERN_LEN 100

void search_pattern_parallel(char *sequence, char *pattern, int seq_len, int pat_len, int rank, int size) {
    int chunk = seq_len / size;
    int start = rank * chunk;
    int end = (rank == size - 1) ? seq_len : start + chunk + pat_len - 1;

    printf("Process %d searching from %d to %d\n", rank, start, end);

    for (int i = start; i <= end - pat_len; i++) {
        if (strncmp(&sequence[i], pattern, pat_len) == 0) {
            printf("Process %d found pattern at index %d\n", rank, i);
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 3) {
        if (rank == 0)
            printf("Usage: %s <sequence_file> <pattern>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    char *pattern = argv[2];
    int pat_len = strlen(pattern);
    char *sequence = malloc(MAX_SEQ_LEN);

    if (rank == 0) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fscanf(file, "%s", sequence);
        fclose(file);
    }

    MPI_Bcast(&pat_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(pattern, pat_len, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(sequence, MAX_SEQ_LEN, MPI_CHAR, 0, MPI_COMM_WORLD);

    search_pattern_parallel(sequence, pattern, strlen(sequence), pat_len, rank, size);

    free(sequence);
    MPI_Finalize();
    return 0;
}
