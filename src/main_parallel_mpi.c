#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "utils.h"

// Funzione di ricerca del pattern nella porzione assegnata
int search_pattern_local(char *local_sequence, char *pattern, int local_len, int pat_len) {
    int matches = 0;

    // Cerca nella porzione assegnata (con overlap)
    for (int i = 0; i <= local_len - pat_len; i++) {
        if (strncmp(&local_sequence[i], pattern, pat_len) == 0) {
            matches++;
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

    // Broadcast sequence length and pattern info
    MPI_Bcast(&seq_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&pat_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(pattern, pat_len, MPI_CHAR, 0, MPI_COMM_WORLD);

    int chunk = seq_len / size;
    int start = rank * chunk;
    int end = (rank == size - 1) ? seq_len : start + chunk + pat_len - 1;
    int local_len = end - start;

    char *local_sequence = malloc(local_len + 1);
    if (!local_sequence) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int *sendcounts = NULL;
    int *displs = NULL;

    if (rank == 0) {
        sendcounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));
        for (int i = 0; i < size; i++) {
            int st = i * chunk;
            int en = (i == size - 1) ? seq_len : st + chunk + pat_len - 1;
            sendcounts[i] = en - st;
            displs[i] = st;
        }
    }

    MPI_Scatterv(sequence, sendcounts, displs, MPI_CHAR, local_sequence, local_len, MPI_CHAR, 0, MPI_COMM_WORLD);
    local_sequence[local_len] = '\0';

    // Misura il tempo solo in rank 0
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    // Ogni processo esegue la ricerca
    int local_matches = search_pattern_local(local_sequence, pattern, local_len, pat_len);

    // Riduce il numero di match totali su rank 0
    int total_matches = 0;
    MPI_Reduce(&local_matches, &total_matches, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Misura il tempo solo in rank 0 e stampa i risultati
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

    if (rank == 0) {
        free(sequence);
        free(sendcounts);
        free(displs);
    }
    free(local_sequence);
    MPI_Finalize();
    return EXIT_SUCCESS;
}
