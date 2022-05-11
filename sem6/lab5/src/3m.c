#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define FILENAME "out.txt"

void *run(void *args) {
    FILE *f = fopen(FILENAME, "w");

    for (char c = 'b'; c <= 'z'; c += 2) {
        fprintf(f, "%c", c);
    }

    fclose(f);

    return NULL;
}

int main() {
    FILE *f1 = fopen(FILENAME, "w");

    pthread_t td;
    pthread_create(&td, NULL, run, NULL);

    for (char c = 'a'; c <= 'z'; c += 2) {
        fprintf(f1, "%c", c);
    }

    pthread_join(td, NULL);
    fclose(f1);

    return 0;
}
