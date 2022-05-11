#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#define BUF_SIZE 20
#define FILENAME "alphabet.txt"

void *run(void *args) {
    int *fd = (int *)args;
    FILE *fs2 = fdopen(*fd, "r");
    char buff2[BUF_SIZE];
    setvbuf(fs2, buff2, _IOFBF, BUF_SIZE);
    int flag = 1;
    char c;
    while ((flag = fscanf(fs2, "%c", &c)) == 1) {
        fprintf(stdout, "%c", c);
    }
    return NULL;
}

int main() {
    pthread_t td;
    int fd = open(FILENAME, O_RDONLY);

    FILE *fs1 = fdopen(fd, "r");
    char buff1[BUF_SIZE];
    setvbuf(fs1, buff1, _IOFBF, BUF_SIZE);
    pthread_create (&td, NULL, run, &fd);
    int flag = 1;
    char c;
    while ((flag = fscanf(fs1, "%c", &c)) == 1) {
        fprintf(stdout, "%c", c);
    }
    pthread_join(td, NULL);
    return 0;
}
