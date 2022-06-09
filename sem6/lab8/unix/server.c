#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "socket.h"

void Cleanup(int sock) {
    close(sock);
    unlink(SOCK_NAME);
}

int main(int argc, char *argv[]) {
    struct sockaddr srvr_name;
    struct sockaddr rcvr_name;
    int namelen;

    char buf[MAX_MSG_LEN];

    int sock;
    int bytes;

    long int curr_time = time(NULL);

    // AF_UNIX - сокеты в файловом пространстве имен.
    // SOCK_DGRAM - датаграмный сокет. Хранит границы сообщений.
    sock = socket(AF_UNIX, SOCK_DGRAM, SOCK_STREAM);
    if (sock < 0) {
        perror("socket failed");
        return ERROR_CREATE_SOCKET;
    }

    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCK_NAME);

    rcvr_name.sa_family = AF_UNIX;
    strcpy(rcvr_name.sa_data, SOCK_NAME);

    if (bind(sock, &srvr_name, LEN_STRUCT_SOCKADDR(srvr_name)) < 0) {
        perror("bind failed");
        return ERROR_BIND_SOCKET;
    }

    while (TRUE) {
        bytes = recvfrom(sock, buf, sizeof(buf), 0, &rcvr_name, &namelen);

        if (bytes < 0) {
            perror("recvfrom failed");
            Cleanup(sock);
            return ERROR_RECVFROM_SOCKET;
        }

        printf("\n\nReceived message: %s" WHITE "\nLen = %ld\n______", buf,
               strlen(buf));

        if (sendto(sock, buf, strlen(buf) + 1, 0, &rcvr_name,
                   LEN_STRUCT_SOCKADDR(rcvr_name)) < 0) {
            perror("sendto failed");
            return ERROR_SENDTO_SOCKET;
        }
    }

    Cleanup(sock);

    return OK;
}
