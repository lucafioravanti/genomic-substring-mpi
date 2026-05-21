#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Load entire file (multi-line) into a single dynamically allocated string
static inline char *load_sequence(const char *filename) {
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

#endif // UTILS_H
