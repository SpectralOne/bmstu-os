#include <dirent.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "const.h"

#define BUF_SIZE 10000

#define NO_ACCESS 1
#define BAD_ARGS 2

void print_cmdline(const int pid) {
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%d/cmdline", pid);
    FILE *file = fopen(path, "r");

    char buf[BUF_SIZE];
    int len = fread(buf, 1, BUF_SIZE, file);
    buf[len - 1] = 0;
    printf("\nCMDLINE:\n%s\n", buf);

    fclose(file);
}

void print_environ(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/environ", pid);
    FILE *file = fopen(pathToOpen, "r");

    int len;
    char buf[BUF_SIZE];
    printf("\nENVIRON:\n");
    while ((len = fread(buf, 1, BUF_SIZE, file)) > 0) {
        for (int i = 0; i < len; i++)
            if (!buf[i]) buf[i] = '\n';
        buf[len - 1] = '\n';
        printf("%s", buf);
    }

    fclose(file);
}

void print_fd(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/fd/", pid);
    DIR *dir = opendir(pathToOpen);

    printf("\nFD:\n");

    struct dirent *readDir;
    char string[PATH_MAX];
    char path[BUF_SIZE] = {'\0'};
    while ((readDir = readdir(dir)) != NULL) {
        if ((strcmp(readDir->d_name, ".") != 0) &&
            (strcmp(readDir->d_name, "..") != 0)) {
            sprintf(path, "%s%s", pathToOpen, readDir->d_name);
            int _read_len = readlink(path, string, PATH_MAX);
            printf("%s -> %s\n", readDir->d_name, string);
        }
    }

    closedir(dir);
}

void print_stat(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/stat", pid);
    char buf[BUF_SIZE];

    FILE *file = fopen(pathToOpen, "r");
    fread(buf, 1, BUF_SIZE, file);
    char *token = strtok(buf, " ");

    printf("\nSTAT: \n");

    for (int i = 0; token != NULL; i++) {
        printf(NO_DESCR[i], token);
        token = strtok(NULL, " ");
    }

    fclose(file);
}

void print_statm(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/statm", pid);
    FILE *file = fopen(pathToOpen, "r");
    char buf[BUF_SIZE];
    fread(buf, 1, BUF_SIZE, file);

    char *token = strtok(buf, " ");
    printf("\nSTATM: \n");
    for (int i = 0; token != NULL; i++) {
        printf(STATM_PATTERNS[i], token);
        token = strtok(NULL, " ");
    }
    fclose(file);
}

void print_cwd(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/cwd", pid);
    char buf[BUF_SIZE] = {'\0'};
    int _read_len = readlink(pathToOpen, buf, BUF_SIZE);
    printf("\nCWD:\n");
    printf("%s\n", buf);
}

void print_io(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/io", pid);
    char buf[BUF_SIZE] = {'\0'};
    FILE *file = fopen(pathToOpen, "r");
    printf("\nIO:\n");
    int lengthOfRead;
    while ((lengthOfRead = fread(buf, 1, BUF_SIZE, file))) {
        buf[lengthOfRead] = '\0';
        printf("%s\n", buf);
    }
    fclose(file);
}

void print_comm(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/comm", pid);
    char buf[BUF_SIZE] = {'\0'};
    FILE *file = fopen(pathToOpen, "r");
    printf("\nCOMM:\n");
    int lengthOfRead;
    while ((lengthOfRead = fread(buf, 1, BUF_SIZE, file))) {
        buf[lengthOfRead] = '\0';
        printf("%s\n", buf);
    }
    fclose(file);
}

void print_exe(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/exe", pid);
    char buf[BUF_SIZE] = {'\0'};
    int _read_len = readlink(pathToOpen, buf, BUF_SIZE);
    printf("\nEXE:\n");
    printf("%s\n", buf);
}

void print_maps(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/maps", pid);
    char buf[BUF_SIZE] = {'\0'};
    FILE *file = fopen(pathToOpen, "r");
    printf("\nMAPS:\n");
    int lengthOfRead;
    while ((lengthOfRead = fread(buf, 1, BUF_SIZE, file))) {
        buf[lengthOfRead] = '\0';
        printf("%s\n", buf);
    }
    fclose(file);
}

void print_file(const int num) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/maps", num);
    char buf[BUF_SIZE] = {'\0'};
    FILE *file = fopen(pathToOpen, "r");
    int lengthOfRead;
    while ((lengthOfRead = fread(buf, 1, BUF_SIZE, file))) {
        buf[lengthOfRead] = '\0';
        printf("%s\n", buf);
    }
    fclose(file);
}

void print_task(const int pid) {
    DIR *d;
    struct dirent *dir;

    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/task", pid);

    d = opendir(pathToOpen);
    printf("\nTASK:\n");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
}

void print_page(uint64_t address, uint64_t data) {
    printf("0x%-16lx : %-16lx %-10ld %-10ld %-10ld %-10ld\n", address,
           data & (((uint64_t)1 << 55) - 1), (data >> 55) & 1, (data >> 61) & 1,
           (data >> 62) & 1, (data >> 63) & 1);
}

void get_pagemap_info(const int pid) {
    printf("PAGEMAP\n");
    printf(" addr : pfn soft-dirty file/shared swapped present\n");

    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%d/maps", pid);
    FILE *maps = fopen(path, "r");

    snprintf(path, PATH_MAX, "/proc/%d/pagemap", pid);
    int pm_fd = open(path, O_RDONLY);
    char buf[BUF_SIZE + 1] = "\0";

    int len;

    // чтение maps
    while ((len = fread(buf, 1, BUF_SIZE, maps)) > 0) {
        for (int i = 0; i < len; ++i)
            if (buf[i] == 0) buf[i] = '\n ';
        buf[len] = '\0 ';

        // проход по строкам из maps
        // используем strtok_r вместо strtok для возможности разбиения на
        // лексемы разных буферов
        char *save_row;
        char *row = strtok_r(buf, "\n", &save_row);

        while (row) {
            // получение столбца участка адресного пространства
            char *addresses = strtok(row, " ");
            char *start_str, *end_str;

            // получение начала и конца участка адресного пространства

            if ((start_str = strtok(addresses, "-")) &&
                (end_str = strtok(NULL, "-"))) {
                uint64_t start = strtoul(start_str, NULL, 16);
                uint64_t end = strtoul(end_str, NULL, 16);

                for (uint64_t i = start; i < end; i += sysconf(_SC_PAGE_SIZE)) {
                    uint64_t offset;
                    // поиск смещения, по которому в pagemap находится
                    // информация о текущей странице
                    uint64_t index =
                        i / sysconf(_SC_PAGE_SIZE) * sizeof(offset);

                    pread(pm_fd, &offset, sizeof(offset), index);
                    print_page(i, offset);
                }
            }
            row = strtok_r(NULL, "\n", &save_row);
        }
    }
    fclose(maps);
    close(pm_fd);
}

void print_root(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/root", pid);
    char buf[BUF_SIZE] = {'\0'};

    printf("\nROOT:\n");
    printf("%s\n", buf);
}

int get_pid(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Введите pid процесса для исследования!");
        exit(BAD_ARGS);
    }

    int pid = atoi(argv[1]);
    char check_dir[PATH_MAX];
    snprintf(check_dir, PATH_MAX, "/proc/%d", pid);
    if (!pid || access(check_dir, F_OK)) {
        printf("Директории для введенного pid не найдено");
        exit(NO_ACCESS);
    }

    return pid;
}

int main(int argc, char *argv[]) {
    int pid = get_pid(argc, argv);
    print_cmdline(pid);
    print_fd(pid);
    print_environ(pid);
    print_stat(pid);
    print_statm(pid);
    print_cwd(pid);
    print_exe(pid);
    print_maps(pid);
    print_io(pid);
    print_comm(pid);
    print_root(pid);
    print_task(pid);

    get_pagemap_info(pid);
    return 0;
}
