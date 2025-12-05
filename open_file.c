#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Use: %s <file_name>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "w");
    if (f == NULL) {
        perror("Error while opening");
        return 1;
    }

    printf("Enter text. To exit use Ctrl+F\n");

    int ch;
    while ((ch = fgetc(stdin)) != EOF) {
        if (ch == 6) {
            break;
        }

        if (fputc(ch, f) == EOF) {
            perror("Error while writing");
            if (fclose(f) == EOF) {
                perror("Error while closing");
            }
            return 1;
        }
    }

    if (ferror(stdin)) {
        fprintf(stderr, "Error while writing from keyboard\n");
        if (fclose(f) == EOF) {
            perror("Error while closing");
        }
        return 1;
    }

    if (fclose(f) == EOF) {
        perror("Error while closing");
        return 1;
    }

    return 0;
}
