#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Load entire file (multi-line) into a single dynamically allocated string
char *load_sequence(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    size_t capacity = 1024;  // Initial buffer capacity
    size_t length = 0;       // Current length of the sequence
    char *sequence = malloc(capacity);
    if (!sequence) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    char line[1024];
    // Read file line by line
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if present
        size_t linelen = strlen(line);
        if (linelen > 0 && line[linelen - 1] == '\n') {
            line[linelen - 1] = '\0';
            linelen--;
        }

        // Check if the current buffer can hold the new line plus null terminator
        if (length + linelen + 1 > capacity) {
            // Increase capacity (doubling strategy)
            capacity = (length + linelen + 1) * 2;
            char *temp = realloc(sequence, capacity);
            if (!temp) {
                perror("Memory reallocation failed");
                free(sequence);
                fclose(file);
                return NULL;
            }
            sequence = temp;
        }

        // Append the current line (without newline) to the sequence buffer
        memcpy(sequence + length, line, linelen);
        length += linelen;
    }

    // Null-terminate the full sequence string
    sequence[length] = '\0';

    fclose(file);
    return sequence;
}

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
	int ret = system("python \"C:/Users/lucav/Desktop/ACA-main/src/Graph_TIME.py\"");
    if (ret != 0) {
        fprintf(stderr, "Errore durante l'esecuzione dello script Python.\n");
    }

    free(sequence);
    return EXIT_SUCCESS;
}
