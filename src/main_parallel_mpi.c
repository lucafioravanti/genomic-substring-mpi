#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "utils.h"

// Funzione di ricerca del pattern nella porzione assegnata
int search_pattern_parallel(char *sequence, char *pattern, int seq_len, int pat_len, int rank, int size) {
    int matches = 0;

    int chunk = seq_len / size;
    int start = rank * chunk;
    int end = (rank == size - 1) ? seq_len : start + chunk + pat_len - 1;

    // Cerca nella porzione assegnata (con overlap)
    for (int i = start; i <= end - pat_len; i++) {
        if (strncmp(&sequence[i], pattern, pat_len) == 0) {
            matches++;
            // printf("Process %d found pattern at index %d\n", rank, i); 
        }
    }

    return matches;
}

int main(int argc, char *argv[]) {
    int rank, size;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 3) {
        if (rank == 0)
            printf("Usage: %s <sequence_file> <pattern>\n", argv[0]);
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    char *pattern = argv[2];
    int pat_len = strlen(pattern);
    char *sequence = NULL;
    int seq_len = 0;

    // Rank 0 legge il file e determina la lunghezza
    if (rank == 0) {
        sequence = load_sequence(argv[1]);
        if (!sequence) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        seq_len = strlen(sequence);
    }

    // Broadcast sequence length
    MPI_Bcast(&seq_len, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate memory on all other ranks
    if (rank != 0) {
        sequence = malloc(seq_len + 1);
        if (!sequence) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Broadcast pattern_len, pattern and sequence
    MPI_Bcast(&pat_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(pattern, pat_len, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(sequence, seq_len + 1, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Misura il tempo solo in rank 0
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    // Ogni processo esegue la ricerca
    int local_matches = search_pattern_parallel(sequence, pattern, seq_len, pat_len, rank, size);

    // Riduce il numero di match totali su rank 0
    int total_matches = 0;
    MPI_Reduce(&local_matches, &total_matches, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Rank 0 stampa risultati e tempo
    if (rank == 0) {
        end_time = MPI_Wtime();
        double elapsed = end_time - start_time;

        printf("Total matches: %d\n", total_matches);
        printf("Execution time (MPI): %.6f seconds\n", elapsed);

        // Log su file
        FILE *log = fopen("log_mpi.csv", "a");
        if (log) {
            fprintf(log, "%s,%d,%.6f,%s\n", pattern, total_matches, elapsed, argv[1]);
            fclose(log);
        } else {
            perror("Error writing to log_mpi.csv");
        }
    }

    free(sequence);
    MPI_Finalize();
    return EXIT_SUCCESS;
}
