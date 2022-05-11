#include <fcntl.h>
#include <unistd.h>

#define FILENAME "alphabet.txt"

int main() {
    char c;
    int fd1 = open(FILENAME, O_RDONLY);
    int fd2 = open(FILENAME, O_RDONLY);

    while (1) {
        if (read(fd1, &c, 1) != 1) break;
        write(1, &c, 1);

        if (read(fd2, &c, 1) != 1) break;
        write(1, &c, 1);
    }

    return 0;
}
