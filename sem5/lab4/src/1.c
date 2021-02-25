#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int pid;
int child_pids[2];

int main(void) {
    printf("Parent process: PID=%d, GROUP=%d\n", getpid(), getpgrp());

    for (size_t i = 0; i < 2; ++i) {
        switch (pid = fork()) {
            case -1:
                perror("Can't fork\n");
                return EXIT_FAILURE;
            case 0:
                sleep(2);
                printf("Child process : PID=%d, GROUP=%d, PPID=%d\n", getpid(),
                       getpgrp(), getppid());
                return EXIT_SUCCESS;
            default:
                child_pids[i] = pid;
        }
    }
    printf("Parent process have children with IDs: %d, %d\n", child_pids[0],
           child_pids[1]);
    printf("Parent process is dead now\n");

    return EXIT_SUCCESS;
}
