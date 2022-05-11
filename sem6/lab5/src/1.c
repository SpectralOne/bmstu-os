#include <fcntl.h>
#include <stdio.h>

#define BUF_SIZE 20
#define FILENAME "alphabet.txt"

int main() {
    int fd = open(FILENAME, O_RDONLY);

    FILE* fs1 = fdopen(fd, "r");
    char buff1[BUF_SIZE];
    setvbuf(fs1, buff1, _IOFBF, BUF_SIZE);

    FILE* fs2 = fdopen(fd, "r");
    char buff2[BUF_SIZE];
    setvbuf(fs2, buff2, _IOFBF, BUF_SIZE);

    int flag1 = 1, flag2 = 1;
    while (flag1 == 1 || flag2 == 1) {
        char c;
        flag1 = fscanf(fs1, "%c", &c);
        if (flag1 == 1) fprintf(stdout, "%c", c);

        flag2 = fscanf(fs2, "%c", &c);
        if (flag2 == 1) fprintf(stdout, "%c", c);
    }

    return 0;
}
