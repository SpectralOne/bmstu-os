#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int pid;
int child_pids[2];
const char volatile *msg[] = {"message1", "message2"};

int state = 0;

void ignore_sigint(int sig) {}

void is_writing(int sig) { state = 1; }

int main() {
    int fd[2];
    char buffer[50] = {0};

    printf("Parent process: PID=%d, GROUP=%d\n", getpid(), getpgrp());

    if (pipe(fd) == -1) {
        perror("Can't pipe\n");
        return EXIT_FAILURE;
    }

    signal(SIGINT, ignore_sigint);
    for (size_t i = 0; i < 2; ++i) {
        switch (pid = fork()) {
            case -1:
                perror("Can't fork\n");
                exit(EXIT_FAILURE);
            case 0:
                signal(SIGINT, is_writing);
                sleep(5);
                if (state) {
                    close(fd[0]);
                    write(fd[1], msg[i], strlen(msg[i]));
                    printf("Message has been sent to parent\n");
                } else {
                    printf("No signal sent, writing will not be completed\n");
                }

                exit(EXIT_SUCCESS);
            default:
                child_pids[i] = pid;
        }
    }

    for (size_t i = 0; i < 2; ++i) {
        int status;
        pid_t childpid = wait(&status);
        printf("Child process finished: PID = %d, status = %d\n", childpid, status);

        int stat_val;
        if (WIFEXITED(stat_val)) {
            printf("Child process exited with code %d\n",
                   WEXITSTATUS(stat_val));
        } else {
            printf("Child process terminated abnormally\n");
        }
    }

    close(fd[1]);
    read(fd[0], buffer, sizeof(buffer));

    printf("Received message: %s\n", buffer);
    printf("Parent process have children with IDs: %d, %d\n", child_pids[0],
           child_pids[1]);
    printf("Parent process is dead now\n");

    return EXIT_SUCCESS;
}