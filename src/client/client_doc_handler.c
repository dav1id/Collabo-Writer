#include "../../include/client_doc_controller.h"
#include "../../include/utils.h"

#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>

#include <string.h>
#include <sys/errno.h>

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

void client_write_server(const char* doc_name, int remote_sock) {
    char path_name[64] = "data/";
    snprintf(path_name + strlen("data/"), sizeof(path_name) - strlen("data/"), "%s", doc_name);

    FILE *fp = fopen(path_name, 'r');

    int byte_length; int num_segments;
    int segment_incr = 0;

    fseek(fp, 0, SEEK_END);
    byte_length = (int) ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // send some metadata here, i.e. number of segments
    send();

    while (segment_incr < num_segments) {

    }
}

void server_write_client(const char* doc_name, int remote_sock) {
    char path_name[64] = "data/";
    snprintf(path_name + strlen("data/"), sizeof(path_name) - strlen("data/"), "%s", doc_name);

    FILE *fp = fopen(path_name, 'r');
}