#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_SEQ_LEN 1000000      // Dimensione massima della sequenza
#define MAX_PATTERN_LEN 100      // Lunghezza massima del pattern

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
    char *sequence = malloc(MAX_SEQ_LEN);

    // Rank 0 legge il file
    if (rank == 0) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fscanf(file, "%s", sequence);
        fclose(file);
    }

    // Broadcast pattern e pattern_len
    MPI_Bcast(&pat_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(pattern, pat_len, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(sequence, MAX_SEQ_LEN, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Misura il tempo solo in rank 0
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    // Ogni processo esegue la ricerca
    int local_matches = search_pattern_parallel(sequence, pattern, strlen(sequence), pat_len, rank, size);

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
