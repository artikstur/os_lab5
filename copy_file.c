#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Use: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    struct stat info;
    
    if (stat(argv[1], &info) != 0) {
        perror("Error reading source file properties");
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (in == NULL) {
        perror("Error opening source file");
        return 1;
    }

    FILE *out = fopen(argv[2], "w");
    if (out == NULL) {
        perror("Error creating destination file");
        fclose(in);
        return 1;
    }

    int c;
    while ((c = getc(in)) != EOF) {
        if (putc(c, out) == EOF) {
            perror("Error writing to destination file");
            fclose(in);
            fclose(out);
            return 1;
        }
    }

    if (ferror(in)) {
        fprintf(stderr, "Error reading from source file\n");
        fclose(in);
        fclose(out);
        return 1;
    }

    if (fclose(in) != 0) {
        perror("Error closing source file");
        fclose(out);
        return 1;
    }

    if (fclose(out) != 0) {
        perror("Error closing destination file");
        return 1;
    }

    if (chmod(argv[2], info.st_mode) != 0) {
        perror("Error copying file permissions");
        return 1;
    }

    printf("File '%s' successfully copied to '%s'\n", argv[1], argv[2]);
    return 0;
}
