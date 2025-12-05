#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int validate_directory(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Directory opening error");
        return 0; 
    }
    closedir(dir);
    return 1; 
}

void print_directory(const char *path) {
    printf("\nDirectory: %s\n", path);

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("  %s\n", entry->d_name);
    }

    if (closedir(dir) != 0) {
        perror("Error closing directory");
    }
}

int main(int argc, char *argv[]) {
    if (validate_directory(".")) {
        print_directory(".");
    }

    if (argc == 2) {
        if (validate_directory(argv[1])) {
            print_directory(argv[1]);
        }
    } else if (argc > 2) {
        fprintf(stderr, "Use: %s [directory]\n", argv[0]);
        return 1;
    }

    return 0;
}
