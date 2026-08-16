#include "../../include/utils.h"

#include <stdio.h>
#include <string.h>

#include <netdb.h>
#include <sys/errno.h>


typedef struct addrinfo addrinfo;

int recv_file_contents(const char* dest, int sock) {
    FILE *fp;
    if ((fp = fopen(dest, "wb")) == NULL) {
        printf("Error Number: %d: %s\n", errno, strerror(errno));
        return -1;
    }

    char request[SEGMENT_LEN];

    int bytes_recv = (int) recv(sock, request, SEGMENT_LEN, 0);

    printf("Received %d bytes from a remote client socket....\n", bytes_recv);

    printf("%d\n", (int) strlen(request));

    //fprintf(fp, "%*.s\n", (int) strlen(request) - 1,  request )
    if (fprintf(fp, request ) < 0) {
        printf("Error code %d: %s\n", errno, strerror(errno));
    }

    return 0;
}

int main() {
    const char *dest_path = "../../src/config/config_files/out.txt";

    addrinfo *bind_addr;
    addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AI_FAMILY;
    hints.ai_socktype = AI_SOCKTYPE;
    hints.ai_protocol =  AI_PROTOCOL;
    hints.ai_flags = AI_PASSIVE;

    int res = getaddrinfo(NULL, "9096", &hints, &bind_addr);
    VERIFY_RSLT_RTRN(res, "getaddrinfo");

    int listen_sock = socket(AI_FAMILY, AI_SOCKTYPE, AI_PROTOCOL);
    VERIFY_RSLT_RTRN(listen_sock, "socket creation");

    res = bind(listen_sock, bind_addr->ai_addr, bind_addr->ai_addrlen);
    VERIFY_RSLT_RTRN(res, "Socket binding");

    listen(listen_sock, 1);

    struct sockaddr_storage temp_sock_addr;
    socklen_t sockaddr_size = sizeof(temp_sock_addr);

    printf("%s\n", "Waiting for a Client to connect to the server...");
    int remote_sock = accept(listen_sock, (struct sockaddr *) &temp_sock_addr, &sockaddr_size);
    printf("%s\n", "Client has connected to the server...");

    VERIFY_RSLT_RTRN(remote_sock, "accepting remote socket");

    recv_file_contents(dest_path, remote_sock);

    return 1;
}