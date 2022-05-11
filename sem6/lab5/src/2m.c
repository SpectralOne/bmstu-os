#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

#define FILENAME "alphabet.txt"

void *run(void *args) {
    int fd = open(FILENAME, O_RDONLY);
    int flag = 1;
    char c;
    while ((flag = read(fd, &c, 1)) == 1) {
        write(1, &c, 1);
    }
    return NULL;
}

int main() {
    int fd1 = open(FILENAME, O_RDONLY);
    pthread_t td;
    pthread_create(&td, NULL, run, NULL);

    int flag = 1;
    char c;
    while ((flag = read(fd1, &c, 1)) == 1) {
        write(1, &c, 1);
    }

    pthread_join(td, NULL);

    return 0;
}
