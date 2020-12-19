#include "io.h"

struct sembuf READER_QUEUE[] = {
    {READ_QUEUE, 1, 0},
    {WRITER, 0, 0},
    {WRITE_QUEUE, 0, 0},
};

struct sembuf READER_LOCK[] = {
    {READER, 1, 0},
    {READ_QUEUE, -1, 0},
};

struct sembuf READER_RELEASE[] = {
    {READER, -1, 0},
};

struct sembuf WRITER_QUEUE[] = {
    {WRITE_QUEUE, 1, 0},
    {READER, 0, 0},
    {WRITER, 0, 0},
};

struct sembuf WRITER_LOCK[] = {
    {WRITER, 1, 0},
    {WRITE_QUEUE, -1, 0},
};

struct sembuf WRITER_RELEASE[] = {
    {WRITER, -1, 0},
};

static inline int start_read(int sid) {
    return semop(sid, READER_QUEUE, 3) != -1 &&
           semop(sid, READER_LOCK, 2) != -1;
}
static inline int stop_read(int sid) {
    return semop(sid, READER_RELEASE, 1) != -1;
}

int reader_run(int *const shared_counter, 
               const int sid, 
               const int reader_id) {
    if (!shared_counter) {
        return -1;
    }

    srand(time(NULL) + reader_id);

    int sleep_time;
    for (short i = 0; i < ITERATIONS; ++i) {
        sleep_time = rand() % MAX_RANDOM + 1;
        sleep(sleep_time);

        if (!start_read(sid)) {
            perror("Something went wrong with start_read!");
            exit(EXIT_FAILURE);
        }

        // !!! --- CRITICAL --- !!!
        int val = *shared_counter;
        printf(" Reader #%d read:  %3d (slept %ds)\n", reader_id,
               val, sleep_time);
        // !!! --- CRITICAL --- !!!

        if (!stop_read(sid)) {
            perror("Something went wrong with stop_read!");
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}

static inline int start_write(int sid) {
    return semop(sid, WRITER_QUEUE, 3) != -1 &&
           semop(sid, WRITER_LOCK, 2) != -1;
}

static inline int stop_write(int sid) {
    return semop(sid, WRITER_RELEASE, 1) != -1;
}

int writer_run(int *const shared_counter, 
               const int sid, 
               const int writer_id) {
    if (!shared_counter) {
        return -1;
    }

    srand(time(NULL) + writer_id + READERS_COUNT);

    int sleep_time;
    for (short i = 0; i < ITERATIONS; ++i) {
        sleep_time = rand() % MAX_RANDOM + 1;
        sleep(sleep_time);

        if (!start_write(sid)) {
            perror("Something went wrong with start_write!");
            exit(EXIT_FAILURE);
        }

        // !!! --- CRITICAL --- !!!
        int val = ++(*shared_counter);
        printf(" Writer #%d write: %3d (slept %ds)\n", writer_id,
               val, sleep_time);
        // !!! --- CRITICAL --- !!!
        
        if (!stop_write(sid)) {
            perror("Something went wrong with stop_write!");
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
