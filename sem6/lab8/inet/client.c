#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define SOCKET_PORT 8888
#define BUF_SIZE 256

int main(void) {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket call error");
        return errno;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(SOCKET_PORT);
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

    if (connect(sfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect call error");
        return errno;
    }

    char msg[BUF_SIZE];
    for(;;) {
        snprintf(msg, BUF_SIZE, "Hello from process #%d", getpid());
        /*if (sendto(sfd, msg, strlen(msg), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1) {*/
        if (send(sfd, msg, strlen(msg), 0) == -1) {
            perror("sendto call error");
            return errno;
        }
        printf("Send message: %s\n", msg);
        sleep(3);
    }
}
