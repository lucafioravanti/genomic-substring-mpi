#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"

// Naive substring search
int search_pattern_serial(const char *sequence, const char *pattern) {
    int seq_len = strlen(sequence);
    int pat_len = strlen(pattern);
    int matches = 0;

    printf("Pattern found at positions: ");
    for (int i = 0; i <= seq_len - pat_len; i++) {
        if (strncmp(&sequence[i], pattern, pat_len) == 0) {
            printf("%d ", i);
            matches++;
        }
    }
    printf("\n");
    return matches;
}

// Time wrapper for search
double measure_search_time(const char *sequence, const char *pattern, int *match_count) {
    clock_t start = clock();
    *match_count = search_pattern_serial(sequence, pattern);
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

// Main
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <sequence_file> <pattern>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *sequence = load_sequence(argv[1]);
    if (!sequence) {
        return EXIT_FAILURE;
    }

    int match_count = 0;
    double elapsed = measure_search_time(sequence, argv[2], &match_count);

    printf("Total matches: %d\n", match_count);
    printf("Execution time: %.6f seconds\n", elapsed);

        // Log su file CSV
    FILE *log = fopen("log.csv", "a");  // Append mode
    if (log) {
        fprintf(log, "%s,%d,%.6f,%s\n", argv[2], match_count, elapsed, argv[1]);
        fclose(log);
    } else {
        perror("Error writing log.csv");
    }
 //Script Python 
	int ret = system("python src/Graph_TIME.py");
    if (ret != 0) {
        fprintf(stderr, "Errore durante l'esecuzione dello script Python.\n");
    }

    free(sequence);
    return EXIT_SUCCESS;
}
