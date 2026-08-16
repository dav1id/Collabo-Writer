#include "../../include/utils.h"

#include <stdio.h>
#include <string.h>

#include <netdb.h>
#include <sys/errno.h>

int recv_file_contents(const char* dest, int sock) {
    FILE *fp;
    if ((fp = fopen(dest, "wb")) == NULL) {
        printf("Error Number: %d: %s\n", errno, strerror(errno));
        return -1;
    }

    int bytes_recv;
    char request[SEGMENT_LEN];

    while ( (bytes_recv = recv(sock, request, SEGMENT_LEN, 0) != 0) ){
        printf("Received %d bytes from a remote client socket....\n", bytes_recv);

        if (strlen(request) != SEGMENT_LEN) {
            char *request_concat = request;
            fprintf(fp, request_concat);
        } else {
            fprintf(fp, request);
        }
    }

    return 0;
}

int main() {

}