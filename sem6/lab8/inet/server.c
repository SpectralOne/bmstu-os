#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define MAX_CLIENTS_AMOUNT 10
#define SOCKET_PORT 8888
#define BUF_SIZE 256

#define MAX_CLIENTS_ERROR 2

int main(void) {
    int clients[MAX_CLIENTS_AMOUNT] = { 0 };
    int sfd;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket call error");
        return errno;
    }
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(SOCKET_PORT)
    };
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
    if (bind(sfd, (struct sockaddr*)&addr, sizeof addr) == -1) {
        close(sfd);
        perror("bind call error");
        return errno;
    }
    if (listen(sfd, MAX_CLIENTS_AMOUNT) == -1) {
        close(sfd);
        perror("listen call error");
        return errno;
    }
    printf("Server is running on %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    char msg[BUF_SIZE];
    fd_set readfds;
    FD_ZERO(&readfds);

    for (;;) {
        FD_SET(sfd, &readfds);
        int max_sd = sfd;
        for (int i = 0; i < MAX_CLIENTS_AMOUNT; ++i) {
            if (clients[i] > 0)
                FD_SET(clients[i], &readfds);
            if (clients[i] > max_sd)
                max_sd = clients[i];
        }

        if (pselect(max_sd + 1, &readfds, NULL, NULL, NULL, NULL) == -1) {
            close(sfd);
            perror("pselect call error");
            return errno;
        }

        if (FD_ISSET(sfd, &readfds)) {
            struct sockaddr_in addr;
            socklen_t len;
            memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
            const int client_fd = accept(sfd, (struct sockaddr*) &addr, &len);
            printf("New connect from %s:%d\n", inet_ntoa(addr.sin_addr), addr.sin_port);
            if (client_fd == -1) {
                close(sfd);
                perror("accept call error");
                exit(errno);
            }
            int not_set = 1;
            for (int i = 0; not_set && i < MAX_CLIENTS_AMOUNT; ++i) {
                if (!clients[i]) {
                    clients[i] = client_fd;
                    printf("New client created #%d (with fd = %d)\n", i, client_fd);
                    not_set = 0;
                }
            }
            if (not_set) {
                close(sfd);
                fprintf(stderr, "Reached max amount of clients, exiting!");
                exit(MAX_CLIENTS_ERROR);
            }
        }

        for (int i = 0; i < MAX_CLIENTS_AMOUNT; ++i) 
            if (clients[i] && FD_ISSET(clients[i], &readfds)) {
                size_t bytes = recv(clients[i], &msg, BUF_SIZE, 0);
                if (!bytes) {
                    fprintf(stderr, "client #%d (with fd = %d) disconnected\n", i, clients[i]);
                    FD_CLR(clients[i], &readfds);
                    close(clients[i]);
                    clients[i] = 0;
                } else {
                    msg[bytes] = '\0';
                    printf("Got message from client #%d (with fd = %d): %s\n", i, clients[i], msg);
                }
            }
    }
}
