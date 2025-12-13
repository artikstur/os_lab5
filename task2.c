#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <time.h>

time_t parse_time(const char *str) {
    int Y, M, D, h, m, s;
    struct tm tm = {0};

    if (sscanf(str, "%d-%d-%d %d:%d:%d",
               &Y, &M, &D, &h, &m, &s) != 6) {
        fprintf(stderr, "Invalid date format\n");
        exit(1);
    }

    tm.tm_year = Y - 1900;
    tm.tm_mon  = M - 1;
    tm.tm_mday = D;
    tm.tm_hour = h;
    tm.tm_min  = m;
    tm.tm_sec  = s;

    return mktime(&tm);
}

int file_matches(const struct stat *st,
                 long min_size, long max_size,
                 time_t min_time, time_t max_time) {
    long size = (long)st->st_size;
    time_t t = st->st_mtime;

    if (size < min_size || size > max_size) return 0;
    if (t < min_time || t > max_time) return 0;
    return 1;
}

void scan_dir(const char *path,
              long min_size, long max_size,
              time_t min_time, time_t max_time,
              FILE *out) {
    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *entry;
    char full[PATH_MAX];

    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        snprintf(full, sizeof(full), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(full, &st) != 0) continue;

        if (S_ISDIR(st.st_mode)) {
            scan_dir(full, min_size, max_size, min_time, max_time, out);
        } else if (S_ISREG(st.st_mode)) {
            if (file_matches(&st, min_size, max_size, min_time, max_time)) {
                char timebuf[64];
                struct tm *tm_info = localtime(&st.st_mtime);
                strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);
                fprintf(out, "%s %ld %s\n", full, (long)st.st_size, timebuf);
                printf("%s %ld %s\n", full, (long)st.st_size, timebuf);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr,
                "Use: %s <start_dir> <output_file> <N1> <N2> "
                "<YYYY-MM-DD HH:MM:SS> <YYYY-MM-DD HH:MM:SS>\n",
                argv[0]);
        return 1;
    }

    const char *start_dir = argv[1];
    const char *out_name = argv[2];

    long N1 = atol(argv[3]);
    long N2 = atol(argv[4]);
    time_t M1 = parse_time(argv[5]);
    time_t M2 = parse_time(argv[6]);

    if (N2 < N1 || M2 < M1) return 1;

    FILE *out = fopen(out_name, "w");
    if (!out) return 1;

    scan_dir(start_dir, N1, N2, M1, M2, out);

    fclose(out);
    return 0;
}