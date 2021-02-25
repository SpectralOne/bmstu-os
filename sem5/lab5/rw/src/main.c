#include <sys/shm.h>
#include <sys/stat.h>
#include <wait.h>

#include "io.h"

int main(void) {
    setbuf(stdout, NULL);
    int fd = shmget(IPC_PRIVATE, 
                    sizeof(int), 
                    IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd == -1) {
        perror("shmget failed!");
        return EXIT_FAILURE;
    }

    int *shared_counter;
    if ((shared_counter = shmat(fd, 0, 0)) == (void *)-1) {
        perror("shmat failed!");
        return EXIT_FAILURE;
    }

    int sid = semget(IPC_PRIVATE, 
                     MAX_SEMS, 
                     IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);
    if (sid == -1) {
        perror("semget failed!");
        return EXIT_FAILURE;
    }

    semctl(sid, READER, SETVAL, 0);
    semctl(sid, WRITER, SETVAL, 0);
    semctl(sid, WRITE_QUEUE, SETVAL, 0);
    semctl(sid, READ_QUEUE, SETVAL, 0);

    int child_pid;
    for (short i = 0; i < READERS_COUNT; ++i) {
        switch ((child_pid = fork())) {
            case -1:
                perror("reader fork failed!");
                exit(EXIT_FAILURE);
                break;
            case 0:
                reader_run(shared_counter, sid, i);
                return EXIT_SUCCESS;
        }
    }

    for (short i = 0; i < WRITERS_COUNT; ++i) {
        switch ((child_pid = fork())) {
            case -1:
                perror("writer fork failed!");
                exit(EXIT_FAILURE);
                break;
            case 0:
                writer_run(shared_counter, sid, i);
                return EXIT_SUCCESS;
        }
    }

    for (short i = 0; i < WRITERS_COUNT + READERS_COUNT; ++i) {
        int status;
        if (wait(&status) == -1) {
            perror("children error!");
            exit(EXIT_FAILURE);
        }

        if (!WIFEXITED(status)) {
            puts("unexpected termination");
        }
    }

    if (shmdt((void *)shared_counter) == -1 ||
        shmctl(fd, IPC_RMID, NULL) == -1 || 
        semctl(sid, IPC_RMID, 0) == -1) {
            
        perror("exit error!");

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
