#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#include "buffer.h"
#include "runners.h"

#define MAX_SEMS 3

int main(void) {
    setbuf(stdout, NULL);
    int fd = shmget(IPC_PRIVATE, 
                    sizeof(cycle_buff_t),
                    IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd == -1) {
        perror("shmget failed!");
        return EXIT_FAILURE;
    }

    cycle_buff_t *buffer;
    if ((buffer = shmat(fd, 0, 0)) == (void *)-1) {
        perror("shmat failed!");
        return EXIT_FAILURE;
    }

    if (buff_init(buffer) == -1) {
        perror("init failed!");
        return EXIT_FAILURE;
    }

    int sid = semget(IPC_PRIVATE, 
                     MAX_SEMS,
                     IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);
    if (sid == -1) {
        perror("semget failed!");
        return EXIT_FAILURE;
    }

    semctl(sid, BIN_SEM, SETVAL, 1);
    semctl(sid, BUF_EMPTY, SETVAL, N);
    semctl(sid, BUF_FULL, SETVAL, 0);

    int child_pid;
    for (short i = 0; i < PROD_COUNT; ++i) {
        switch ((child_pid = fork())) {
            case -1:
                perror("producer fork failed!");
                exit(EXIT_FAILURE);
                break;
            case 0:
                run_producer(buffer, sid, i);
                return EXIT_SUCCESS;
        }
    }

    for (short i = 0; i < CONS_COUNT; ++i) {
        switch ((child_pid = fork())) {
            case -1:
                perror("consumer fork failed!");
                exit(EXIT_FAILURE);
                break;
            case 0:
                run_consumer(buffer, sid, i);
                return EXIT_SUCCESS;
        }
    }

    for (short i = 0; i < CONS_COUNT + PROD_COUNT; ++i) {
        int status;
        if (wait(&status) == -1) {
            perror("children error!");
            exit(EXIT_FAILURE);
        }
        if (!WIFEXITED(status))
            puts("unexpected termination");
    }

    if (shmdt((void *)buffer) == -1 || 
        shmctl(fd, IPC_RMID, NULL) == -1 ||
        semctl(sid, IPC_RMID, 0) == -1) {
        perror("exit error!");

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
