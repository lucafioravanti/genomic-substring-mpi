#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "utils.h"

#define MAX_MATCHES 10000000

// Ricerca parallela ottimizzata: nessun critical, uso di buffer locali
int search_pattern_parallel(const char *sequence, const char *pattern, int *positions) {
    int seq_len = strlen(sequence);
    int pat_len = strlen(pattern);
    int total_matches = 0;

    int max_threads = omp_get_max_threads();
    int *thread_matches = calloc(max_threads, sizeof(int));
    int **thread_positions = malloc(max_threads * sizeof(int *));

    if (!thread_matches || !thread_positions) {
        perror("Memory allocation failed");
        exit(1);
    }

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int local_count = 0;
        size_t local_capacity = MAX_MATCHES / omp_get_num_threads();
        if (local_capacity == 0) local_capacity = 1024; // fallback

        int *local_positions = malloc(sizeof(int) * local_capacity);
        if (!local_positions) {
            fprintf(stderr, "Memory allocation failed for thread %d\n", tid);
            exit(1);
        }

        #pragma omp for schedule(static)
        for (int i = 0; i <= seq_len - pat_len; i++) {
            if (strncmp(&sequence[i], pattern, pat_len) == 0) {
                if (local_count >= local_capacity) {
                    local_capacity *= 2;
                    int *temp = realloc(local_positions, sizeof(int) * local_capacity);
                    if (!temp) {
                        fprintf(stderr, "Memory reallocation failed for thread %d\n", tid);
                        free(local_positions);
                        exit(1);
                    }
                    local_positions = temp;
                }
                local_positions[local_count++] = i;
            }
        }

        thread_positions[tid] = local_positions;
        thread_matches[tid] = local_count;
    }

    // Fondere i risultati
    for (int t = 0; t < max_threads; t++) {
        if (thread_matches[t] > 0) {
            memcpy(&positions[total_matches], thread_positions[t], thread_matches[t] * sizeof(int));
            total_matches += thread_matches[t];
        }
        if (thread_positions[t]) {
            free(thread_positions[t]);
        }
    }

    free(thread_matches);
    free(thread_positions);

    return total_matches;
}

double measure_search_time_parallel(const char *sequence, const char *pattern, int *positions, int *match_count) {
    double start = omp_get_wtime();
    *match_count = search_pattern_parallel(sequence, pattern, positions);
    double end = omp_get_wtime();
    return end - start;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <sequence_file> <pattern>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *sequence = load_sequence(argv[1]);
    if (!sequence) return EXIT_FAILURE;

    int *positions = malloc(sizeof(int) * MAX_MATCHES);
    if (!positions) {
        perror("Position allocation failed");
        free(sequence);
        return EXIT_FAILURE;
    }

    int match_count = 0;
    double elapsed = measure_search_time_parallel(sequence, argv[2], positions, &match_count);

    printf("Total matches: %d\n", match_count);
    printf("Execution time: %.6f seconds\n", elapsed);

    // (Opzionale) stampa le posizioni
    // printf("Positions: ");
    // for (int i = 0; i < match_count; i++) {
    //     printf("%d ", positions[i]);
    // }
    // printf("\n");

    // Logging
    FILE *log = fopen("log_parallel.csv", "a");
    if (log) {
        fprintf(log, "%s,%d,%.6f,%s\n", argv[2], match_count, elapsed, argv[1]);
        fclose(log);
    } else {
        perror("Error writing log.csv");
    }

    free(positions);
    free(sequence);
    return EXIT_SUCCESS;
}
