#ifndef _SOCKET_H_
#define _SOCKET_H_

#define SOCK_NAME "socket.soc"
#define SOCK_NAME_CLIENT "socket_c.soc"

#define MAX_MSG_LEN 128
#define MAX_LEN_ERR_MSG 256

#define LEN_STRUCT_SOCKADDR(a) strlen(a.sa_data) + sizeof(a.sa_family)

#define RED "\33[31m"
#define GREEN "\33[32m"
#define YELLOW "\33[33m"
#define BLUE "\33[34m"
#define WHITE "\33[37m"

#define TRUE 1
#define FALSE 0

#define OK 0

#define ERROR_CREATE_SOCKET 1
#define ERROR_BIND_SOCKET 2
#define ERROR_RECVFROM_SOCKET 3
#define ERROR_SENDTO_SOCKET 4

#endif