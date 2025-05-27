#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_SEQ_LEN 1000000      // Maximum size of the sequence
#define MAX_PATTERN_LEN 100      // Maximum size of the pattern

void search_pattern_parallel(char *sequence, char *pattern, int seq_len, int pat_len, int rank, int size) {
    // Divide the sequence among processes (simple block decomposition)
    int chunk = seq_len / size;           // Approximate chunk size
    int start = rank * chunk;             // Start index for this process
    int end = (rank == size - 1) ? seq_len : start + chunk + pat_len - 1;

    // Add (pat_len - 1) to avoid missing matches across chunk boundaries

    printf("Process %d searching from %d to %d\n", rank, start, end);

    // Each process searches its assigned chunk
    for (int i = start; i <= end - pat_len; i++) {
        if (strncmp(&sequence[i], pattern, pat_len) == 0) {
            printf("Process %d found pattern at index %d\n", rank, i);
        }
    }
}


int main(int argc, char *argv[]) {
    int rank, size;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the current process rank and the total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Check for correct command-line arguments (done only by rank 0)
    if (argc < 3) {
        if (rank == 0)
            printf("Usage: %s <sequence_file> <pattern>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    char *pattern = argv[2];                  // Pattern to search
    int pat_len = strlen(pattern);            // Length of the pattern
    char *sequence = malloc(MAX_SEQ_LEN);     // Allocate buffer for the sequence


    // Rank 0 reads the input sequence from file
    if (rank == 0) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file");
            MPI_Abort(MPI_COMM_WORLD, 1);     // Abort all processes if file fails
        }
        fscanf(file, "%s", sequence);         // Read the entire sequence as a single string
        fclose(file);
    }

    // Share pattern length with all processes
    MPI_Bcast(&pat_len, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Broadcast pattern itself
    MPI_Bcast(pattern, pat_len, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Broadcast the full sequence to all processes
    MPI_Bcast(sequence, MAX_SEQ_LEN, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Each process performs its portion of the search
    search_pattern_parallel(sequence, pattern, strlen(sequence), pat_len, rank, size);

    free(sequence);        // Free allocated memory
    MPI_Finalize();        // Shut down the MPI environment
    return 0;
}
