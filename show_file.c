#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Use: %s <file_name> <N>\n", argv[0]);
        fprintf(stderr, "N = 0 -> continuous output, N > 0 -> output by groups of N lines\n");
        return 1;
    }

    const char *filename = argv[1];

    char *endptr;
    long N = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || N < 0) {
        fprintf(stderr, "Error: N must be a non-negative integer\n");
        return 1;
    }

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("Error opening file");
        return 1;
    }

    int ch;
    long line_in_group = 0;

    while ((ch = fgetc(f)) != EOF) {
        if (fputc(ch, stdout) == EOF) {
            perror("Error writing to stdout");
            if (fclose(f) == EOF) {
                perror("Error closing file");
            }
            return 1;
        }

        if (ch == '\n') {
            line_in_group++;
        }

        if (N > 0 && line_in_group >= N) {
            printf("\n--- Press any key to continue (q to quit) ---");
            fflush(stdout); 

            int c = fgetc(stdin);
            if (c == 'q' || c == 'Q') {
                break;
            }

            line_in_group = 0;
            printf("\n");
        }
    }

    if (ferror(f)) {
        fprintf(stderr, "Error reading file\n");
        if (fclose(f) == EOF) {
            perror("Error closing file");
        }
        return 1;
    }

    if (fclose(f) == EOF) {
        perror("Error closing file");
        return 1;
    }

    return 0;
}
