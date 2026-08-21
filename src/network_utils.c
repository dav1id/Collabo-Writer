#include "../include/utils.h"
#include <sys/socket.h>
#include <stdio.h>

typedef struct addrinfo addrinfo;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_storage sockaddr_storage;


void write_with_offset(int max_offset, int sock, FILE *fp) {
    char request[SEGMENT_LEN];

    while (max_offset != 0) {
        int end_len = max_offset < SEGMENT_LEN ? max_offset : SEGMENT_LEN;
        int bytes_recv = 0;

        while (bytes_recv != end_len) { // make sure we're receiving a full segment length
            bytes_recv += (int) recv(sock, request, sizeof(request), 0);
        }
        fwrite(request, sizeof(request), SEGMENT_LEN, fp);
    }
}

void recv_with_offset(int max_offset, int sock, FILE *fp) {
    char response[SEGMENT_LEN];

    while (max_offset != 0) {
        int end_len = max_offset < SEGMENT_LEN ? max_offset : SEGMENT_LEN;
        int bytes_recv = 0;

        while (bytes_recv != end_len) {
            bytes_recv += (int) send(sock, response, sizeof(response), 0);
        }
        fread(response, SEGMENT_LEN, SEGMENT_LEN, fp);
    }
}