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

    fseek(fp, 0, SEEK_END);
    int max_offset = (int) ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char request[SEGMENT_LEN];
    request[0] = SEGMENT_LEN; // char's are already just integers - ascii characters

    send(sock, request, SEGMENT_LEN, 0); // sending metadata first

    write_with_offset(max_offset, sock, request);
}

CONNEX_STAGE server_write_client(const int sock, const char* doc_name) {
    char path_name[64] = "data/";
    snprintf(path_name + strlen("data/"), sizeof(path_name) - strlen("data/"), "%s", doc_name);

    FILE *fp = fopen(path_name, "wb");

    char response[SEGMENT_LEN];
    recv(sock, response, sizeof(response), 0);

    int max_offset = 0;

    if (memcpy(max_offset, response, sizeof(int)) < 0)
        return UPDATE_INCOMPL;

    recv_with_offset(max_offset, sock, response);
    return UPDATE_COMPL;
}