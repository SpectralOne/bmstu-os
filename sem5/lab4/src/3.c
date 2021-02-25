#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int pid;
int child_pids[2];
const char volatile *const cmd[2] = {"ls", "whoami"};

int main() {
    printf("Parent process: PID=%d, GROUP=%d\n", getpid(), getpgrp());

    for (size_t i = 0; i < 2; ++i) {
        switch (pid = fork()) {
            case -1:
                perror("Can't fork\n");
                return EXIT_FAILURE;
            case 0:
                printf("Child process : PID=%d, GROUP=%d, PPID=%d\n\n",
                       getpid(), getpgrp(), getppid());

                switch (execlp(cmd[i], cmd[i], 0)) {
                    case -1:
                        perror("Can't exec\n");
                        return EXIT_FAILURE;
                    case 0:
                        return EXIT_SUCCESS;
                }
            default:
                child_pids[i] = pid;
        }
    }

    for (size_t i = 0; i < 2; ++i) {
        int status;
        pid_t childpid = wait(&status);
        printf("\nChild process finished: PID = %d, status = %d\n", childpid, status);
        int stat_val;
        if (WIFEXITED(stat_val)) {
            printf("Child process exited with code %d\n",
                   WEXITSTATUS(stat_val));
        } else {
            printf("Child process terminated abnormally\n");
        }
    }

    printf("Parent process have children with IDs: %d, %d\n", child_pids[0],
           child_pids[1]);
    printf("Parent process is dead now\n");

    return EXIT_SUCCESS;
}