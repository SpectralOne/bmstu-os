#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "socket.h"

void CreateMessage(char buf[MAX_MSG_LEN], int argc, char *argv[]) {
    long int curr_time = time(NULL);

    sprintf(buf, "\n\n" YELLOW "PID: %d" GREEN "\ntime: %smsg: " BLUE, getpid(),
            ctime(&curr_time));

    if (argc < 2)
        strcat(buf, "Hello world!");
    else
        strcat(buf, argv[1]);
}

int main(int argc, char *argv[]) {
    struct sockaddr srvr_name;
    int sock;

    char buf[MAX_MSG_LEN];
    CreateMessage(buf, argc, argv);
    // printf("%s", buf);

    sock = socket(AF_UNIX, SOCK_DGRAM, SOCK_STREAM);
    if (sock < 0) {
        perror("socket failed");
        return ERROR_CREATE_SOCKET;
    }

    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCK_NAME);

    // 0 - доп флаги.
    if (sendto(sock, buf, strlen(buf) + 1, 0, &srvr_name,
               LEN_STRUCT_SOCKADDR(srvr_name)) < 0) {
        perror("sendto failed");
        return ERROR_SENDTO_SOCKET;
    }

    int bytes;
    int namelen;
    while (TRUE) {
        bytes = recvfrom(sock, buf, sizeof(buf), 0, &srvr_name, &namelen);

        if (bytes < 0) {
            perror("recvfrom failed");
            return ERROR_RECVFROM_SOCKET;
        }

        printf("\n\nReceived message: %s" WHITE "\nLen = %ld\n______", buf,
               strlen(buf));

    }

    close(sock);
    printf("Send!");
    return OK;
}
