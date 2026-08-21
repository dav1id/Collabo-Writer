#include "../../include/client_doc_controller.h"
#include "../../include/utils.h"

#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>

#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>

typedef enum CONNEX_STAGE CONNEX_STAGE;

void connect_docu(const int sock,  const char* doc_name) {
    char request[SEGMENT_LEN] = "connect ";
    int *stage;

    snprintf(request, SEGMENT_LEN, doc_name);

    int res = send(sock, request, SEGMENT_LEN, 0);
    VERIFY_RSLT(res, "Client sending request to connect to the server");

    res = recv(sock, &stage, SEGMENT_LEN, 0);
    VERIFY_RSLT(res, "Requesting server response on client connection...")

    if ((CONNEX_STAGE) *stage == CONNEX_SERVER_INCOMPL) {
        printf("%s\n", "Connection to the server was not successful... Please try again");
    }
}

void client_write_server(const int sock, const char* doc_name) {
    char path_name[64] = "data/";
    snprintf(path_name + strlen("data/"), sizeof(path_name) - strlen("data/"), "%s", doc_name, ".txt");

    FILE *fp = fopen(path_name, "rb");

    int segment_incr = 0;

    fseek(fp, 0, SEEK_END);
    int max_offset = (int) ftell(fp);
    int num_segments = max_offset/SEGMENT_LEN + 1;

    fseek(fp, 0, SEEK_SET);

    char response[SEGMENT_LEN];
    response[0] = SEGMENT_LEN; // char's are already just integers - ascii characters

    send(sock, response, SEGMENT_LEN, 0);

    while (segment_incr < num_segments) {
        fread(response, SEGMENT_LEN, SEGMENT_LEN, fp);
        send(sock, response, sizeof(response), 0);

        segment_incr++;
    }
}

void server_write_client(const int sock, const char* doc_name) {
    char path_name[64] = "data/";
    snprintf(path_name + strlen("data/"), sizeof(path_name) - strlen("data/"), "%s", doc_name);

    FILE *fp = fopen(path_name, "wb");

    char response[SEGMENT_LEN];
    recv(sock, response, sizeof(response), 0);

    int num_segments = atoi(response);
    int segment_incr = 0;

    while (segment_incr < num_segments) {
        recv(sock, response, SEGMENT_LEN, 0);
        fwrite(response, SEGMENT_LEN, SEGMENT_LEN, fp);
        segment_incr++;
    }
}