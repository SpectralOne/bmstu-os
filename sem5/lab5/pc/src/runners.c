#include "runners.h"

struct sembuf PROD_LOCK[2] = {{BUF_EMPTY, -1, 0}, {BIN_SEM, -1, 0}};
struct sembuf PROD_RELEASE[2] = {{BUF_FULL, 1, 0}, {BIN_SEM, 1, 0}};

struct sembuf CONS_LOCK[2] = {{BUF_FULL, -1, 0}, {BIN_SEM, -1, 0}};
struct sembuf CONS_RELEASE[2] = {{BUF_EMPTY, 1, 0}, {BIN_SEM, 1, 0}};

int run_producer(cycle_buff_t *const buffer, 
                 const int sid,
                 const int prod_id) {
    if (!buffer) {
        return -1;
    }

    srand(time(NULL) + prod_id);

    int sleep_time;
    char ch;
    for (short i = 0; i < ITERATIONS_AMOUNT; ++i) {
        sleep_time = rand() % MAX_RANDOM_PROD + 1;
        sleep(sleep_time);

        if (semop(sid, PROD_LOCK, 2) == -1) {
            perror("prod lock error!");
            exit(EXIT_FAILURE);
        }

        // !!! --- CRITICAL --- !!!
        ch = 'a' + (char)(buffer->wpos % 26);
        if (buff_write(buffer, ch) == -1) {
            perror("buffer write error!");
            return EXIT_FAILURE;
        }
        printf(" Producer #%d write: %c -- idle %ds\n", prod_id,
               ch, sleep_time);
        // !!! --- CRITICAL --- !!!

        if (semop(sid, PROD_RELEASE, 2) == -1) {
            perror("prod release error!");
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}

int run_consumer(cycle_buff_t *const buffer, 
                 const int sid, 
                 const int cons_id) {
    if (!buffer) { 
        return -1;
    }

    srand(time(NULL) + cons_id + PROD_COUNT);

    int sleep_time;
    char ch;
    for (short i = 0; i < ITERATIONS_AMOUNT; ++i) {
        sleep_time = rand() % MAX_RANDOM_CONS + 1;
        sleep(sleep_time);

        if (semop(sid, CONS_LOCK, 2) == -1) {
            perror("consumer lock error!");
            exit(EXIT_FAILURE);
        }

        // !!! --- CRITICAL --- !!!
        if (buff_read(buffer, &ch) == -1) {
            perror("buffer read error!");
            return EXIT_FAILURE;
        }
        printf(" Consumer #%d read:  %c -- idle %ds\n", cons_id,
               ch, sleep_time);
        // !!! --- CRITICAL --- !!!

        if (semop(sid, CONS_RELEASE, 2) == -1) {
            perror("consumer release error!");
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}
