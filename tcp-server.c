#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/errno.h>
#include <string.h>

#include <sys/select.h>

/*
    Can print out some information about our host that we may want to use in the future.
    i.e., it's ip address

    Can pass in the host ip address into the getaddrinfo() to generate us socket addresses
    that we are going to eventually try to bind to in socket() and bind()
 */

typedef struct addrinfo addrinfo;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_storage sockaddr_storage;

#define VERIFY_RSLT_RTRN(res, inst){ \
    if(res < 0){\
        printf("%s failed| %d: %s", inst, errno, strerror(errno));\
        return -1; \
    }\
} \

#define VERIFY_RSLT(res, inst){ \
    if(res < 0){\
        printf("%s failed| %d: %s", inst, errno, strerror(errno));\
    }\
} \

#define AI_FAMILY AF_INET
#define AI_SOCKTYPE SOCK_STREAM

int decode_user_selection(char* user_selection);
int transmit_user_selection();

int main(int argc, char* argv[]) {
    addrinfo *hints;
    memset(&hints, 0, sizeof(addrinfo));

    hints->ai_family = AI_FAMILY;
    hints->ai_socktype = AI_SOCKTYPE;

    addrinfo *bind_addr; // explanation of the double pointer for bind_addr
    int res = getaddrinfo(0, "8080", hints, &bind_addr);
    VERIFY_RSLT_RTRN(res, "getaddrinfo");

    const int listen_sock = socket(AI_FAMILY, AI_SOCKTYPE, bind_addr->ai_protocol);
    VERIFY_RSLT_RTRN(listen_sock, "listen");

    res = bind(listen_sock, bind_addr->ai_addr, bind_addr->ai_addrlen);
    VERIFY_RSLT_RTRN(res, "bind");

    res = listen(listen_sock, 2);
    VERIFY_RSLT_RTRN(res, "listen");

    fd_set master; fd_set master_modif;
    FD_SET(listen_sock, &master); FD_SET(stdin, &master);

    int max_sock = listen_sock;

    int exit = 0;
    while (exit == 0) {
        int select = select(max_sock + 1, );
    }
}