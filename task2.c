#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>

int file_matches(const struct stat *st,
                 long min_size, long max_size,
                 long min_time, long max_time) {
    long size = (long)st->st_size;
    long t    = (long)st->st_mtime;

    if (size < min_size || size > max_size) {
        return 0;
    }
    if (t < min_time || t > max_time) {
        return 0;
    }
    return 1;
}

void scan_dir(const char *path,
              long min_size, long max_size,
              long min_time, long max_time,
              FILE *out) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    char full[PATH_MAX];

    while ((entry = readdir(dir)) != NULL) {
        // skip . & ..
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // path + "/" + имя
        snprintf(full, sizeof(full), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(full, &st) != 0) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            scan_dir(full, min_size, max_size, min_time, max_time, out);
        } else if (S_ISREG(st.st_mode)) {
            if (file_matches(&st, min_size, max_size, min_time, max_time)) {
                fprintf(out, "%s %ld %ld\n", full, (long)st.st_size, (long)st.st_mtime);
                printf("%s %ld %ld\n", full, (long)st.st_size, (long)st.st_mtime);
            }
        }
    }

    if (closedir(dir) != 0) {
        perror("closedir");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Use: %s <start_dir> <output_file> <N1> <N2> <M1> <M2>\n", argv[0]);
        return 1;
    }

    const char *start_dir = argv[1];
    const char *out_name  = argv[2];

    long N1 = atol(argv[3]);
    long N2 = atol(argv[4]);
    long M1 = atol(argv[5]);
    long M2 = atol(argv[6]);

    if (N2 < N1 || M2 < M1) {
        fprintf(stderr, "Error: N2 >= N1 and M2 >= M1 should be \n");
        return 1;
    }

    FILE *out = fopen(out_name, "w");
    if (!out) {
        perror("fopen");
        return 1;
    }

    scan_dir(start_dir, N1, N2, M1, M2, out);

    if (fclose(out) != 0) {
        perror("fclose");
        return 1;
    }

    return 0;
}
