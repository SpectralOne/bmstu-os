#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __USE_XOPEN
#define __USE_XOPEN_EXTENDED
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "color.h"

#define __NOT_DIR 1
#define __DIR 2
#define __DENIED 3

static char __color_s[1488];

typedef int __handler_fn_t(const char *filename, const struct stat *st,
                           int depth, int type);

static char *random_color();
static __handler_fn_t work_handler;
static int dopath(const char *filename, int depth, __handler_fn_t *);

int main(const int argc, const char **argv) {
    srand(time(NULL));

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <dir>\n", argv[0]);
        return EXIT_FAILURE;
    }

    return dopath(argv[1], 0, work_handler);
}

static int dopath(const char *filename, int depth, __handler_fn_t *func) {
    struct stat statbuf;

    if (lstat(filename, &statbuf) == -1) {
        /* lstat error */
        return EXIT_FAILURE;
    }

    if (S_ISDIR(statbuf.st_mode) == 0) {
        /* not directory */
        return func(filename, &statbuf, depth, __NOT_DIR);
    }

    int status;
    if ((status = func(filename, &statbuf, depth, __DIR)) != EXIT_SUCCESS) {
        /* directory */
        return status;
    }

    DIR *dp;
    if ((dp = opendir(filename)) == NULL) {
        /* denied */
        return func(filename, &statbuf, depth, __DENIED);
    }

    chdir(filename);
    int dir = (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0);
    struct dirent *dentry;
    while (!dir && (dentry = readdir(dp)) != NULL) {
        // Рязанова хочет убрать это из цикла
        // можно до входа в цикл проверить имя файла
        if (strcmp(dentry->d_name, ".") == 0 ||
            strcmp(dentry->d_name, "..") == 0) {
            continue;
        }

        /* recursive call */
        if ((status = dopath(dentry->d_name, depth + 1, func)) !=
            EXIT_SUCCESS) {
            /* exit on error */
            return status;
        }
    }
    chdir("..");

    if (closedir(dp) == -1) {
        fprintf(stderr, "closedir(%s): %s", filename, strerror(errno));
    }

    return status;
}

static int work_handler(const char *filename, const struct stat *st, int depth,
                        int type) {
    for (int i = 0; i + 1 < depth; ++i) {
        printf("│   ");
    }

    const char *s = "";

    switch (type) {
        case __NOT_DIR:
#ifdef __RND_COLOR
            s = random_color();
#else
            switch (st->st_mode & S_IFMT) {
                case S_IFREG:
                    s = st->st_mode & S_IXUSR ? COLOR_EXE : COLOR_REG;
                    break;
                case S_IFBLK:
                    s = COLOR_BLK;
                    break;
                case S_IFCHR:
                    s = COLOR_CHR;
                    break;
                case S_IFIFO:
                    s = COLOR_FIFO;
                    break;
                case S_IFLNK:
                    s = COLOR_LINK;
                    break;
                case S_IFSOCK:
                    s = COLOR_SOCK;
                    break;
                case S_IFDIR:
                    fprintf(stderr, "Dir file type is not expected\n");
                    return EXIT_FAILURE;
                default:
                    fprintf(stderr, "Unknown file type '%s': %d\n", filename,
                            st->st_mode & S_IFMT);
                    return EXIT_FAILURE;
            }
#endif
            printf("├── %s%s%s %d\n", s, filename, COLOR_RESET, st->st_ino);
            break;
        case __DIR:
            printf("%s%s%s%s %d\n", depth ? "├── " : "", COLOR_DIR, filename,
                   COLOR_RESET, st->st_ino);
            break;
        case __DENIED:
            fprintf(stderr, "Can't read directory %s: %s\n", filename,
                    strerror(errno));
            return EXIT_FAILURE;
        default:
            fprintf(stderr, "Unknown file type: %d\n", type);
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

__always_inline static char *random_color() {
    int bg = rand() % BG_SPEC_SIZE;
    int c = rand() % COLOR_LIST_SIZE;

    *__color_s = 0;
    sprintf(__color_s, "%s %s", COLOR_LIST[c], BG_SPECS[bg]);

    return __color_s;
}
