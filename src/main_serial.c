#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define maximum sequence and pattern sizes
#define MAX_SEQ_LEN 1000000
#define MAX_PATTERN_LEN 100

// Function to perform a naive substring search in a genomic sequence
void search_pattern_serial(const char *sequence, const char *pattern) {
    int seq_len = strlen(sequence);   // Length of the main sequence
    int pat_len = strlen(pattern);    // Length of the pattern to search

    printf("Pattern found at positions: ");

    // Slide a window over the sequence from position 0 to (N - M)
    for (int i = 0; i <= seq_len - pat_len; i++) {
        // Compare substring starting at index i with the pattern
        if (strncmp(&sequence[i], pattern, pat_len) == 0) {
            // Pattern match found at index i
            printf("%d ", i);
        }
    }

    printf("\n"); 
}

int main(int argc, char *argv[]) {
    // Ensure the user provides exactly 2 arguments: <filename> and <pattern>
    if (argc < 3) {
        printf("Usage: %s <sequence_file> <pattern>\n", argv[0]);
        return 1;
    }

    // Attempt to open the sequence file in read mode
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Dynamically allocate memory for the sequence
    char *sequence = malloc(MAX_SEQ_LEN);
    if (!sequence) {
        perror("Memory allocation failed");
        return 1;
    }

    // Read the sequence from the file into memory
    // Assumes the sequence is on a single line with no whitespace
    fscanf(file, "%s", sequence);
    fclose(file);  // Close the file after reading

    // Call the substring search function
    search_pattern_serial(sequence, argv[2]);

    // Free the dynamically allocated memory
    free(sequence);

    return 0; 
}