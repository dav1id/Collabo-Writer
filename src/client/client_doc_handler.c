#include "../../include/client_doc_controller.h"
#include "../../include/utils.h"

#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>

#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>

typedef enum CONNEX_STAGE CONNEX_STAGE;

void connect_docu(int server_sock,  const char* doc_name) {
    char request[SEGMENT_LEN] = "connect ";
    int *stage;

    snprintf(request, SEGMENT_LEN, doc_name);

    int res = send(server_sock, request, SEGMENT_LEN, 0);
    VERIFY_RSLT(res, "Client sending request to connect to the server");

    res = recv(server_sock, &stage, SEGMENT_LEN, 0);
    VERIFY_RSLT(res, "Requesting server response on client connection...")

    if ((CONNEX_STAGE) *stage == CONNEX_SERVER_INCOMPL) {
        printf("%s\n", "Connection to the server was not sucessful... Please try again");
    }
}

void client_write_server(int sock, const char* doc_name) {
    char path_name[64] = "data/";
    snprintf(path_name + strlen("data/"), sizeof(path_name) - strlen("data/"), "%s", doc_name);

    FILE *fp = fopen(path_name, "rb");

    int segment_incr = 0;

    fseek(fp, 0, SEEK_END);
    int byte_length = (int) ftell(fp);
    int num_segments = byte_length/SEGMENT_LEN + 1;

    fseek(fp, 0, SEEK_SET);

    char response[SEGMENT_LEN];

    snprintf(response, sizeof(response), "%d", num_segments); // sending the number of segments to the server first
    send(sock, response, SEGMENT_LEN, 0);

    while (segment_incr < num_segments) {
        fread(response, SEGMENT_LEN, SEGMENT_LEN, fp);
        send(sock, response, sizeof(response), 0);

        segment_incr++;
    }
}

void server_write_client(int sock, const char* doc_name) {
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