#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SEQ_LEN 1000000
#define MAX_PATTERN_LEN 100

void search_pattern_serial(const char *sequence, const char *pattern) {
    int seq_len = strlen(sequence);
    int pat_len = strlen(pattern);

    printf("Pattern found at positions: ");
    for (int i = 0; i <= seq_len - pat_len; i++) {
        if (strncmp(&sequence[i], pattern, pat_len) == 0) {
            printf("%d ", i);
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <sequence_file> <pattern>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char *sequence = malloc(MAX_SEQ_LEN);
    if (!sequence) {
        perror("Memory allocation failed");
        return 1;
    }

    fscanf(file, "%s", sequence);
    fclose(file);

    search_pattern_serial(sequence, argv[2]);

    free(sequence);
    return 0;
}
