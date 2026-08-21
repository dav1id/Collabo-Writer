#include "../../include/doc_fork.h"
#include "../../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/errno.h>
#include <string.h>

#include <sys/select.h>


typedef enum CONNEX_STAGE CONNEX_STAGE;
void server_response_connection(const int sock, const enum CONNEX_STAGE stage) {
    int* int_stage = (int* ) &stage;
    send(sock, &int_stage, sizeof(int), 0);
}

CONNEX_STAGE server_write_self(const int sock, const char* doc_name) {
    char request[SEGMENT_LEN];

    char path[64] = "/data";
    snprintf(path, sizeof(path), "%s", doc_name);

    FILE* fp = fopen(path, "wb");
    fseek(fp, 0, SEEK_SET);

    fseek(fp, 0, SEEK_END);
    int max_offset = (int) ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (memcpy(max_offset, request, sizeof(int)))
        return UPDATE_INCOMPL;

    write_with_offset(max_offset, sock, request);
    return UPDATE_COMPL;
}

CONNEX_STAGE fork_selector_controller(const int sock, char* request, const char* doc_name) {
    char response[SEGMENT_LEN];

    recv(sock, request, SEGMENT_LEN, 0);

    char comp_one[64];
    snprintf(comp_one, sizeof(comp_one), "update %s", doc_name);
    char* comp_two  = &request[strlen(comp_one)];

    /*
        The user wants to update the document. Here we're going to send a message to the client
        saying that we're ready to receive the segment portions of the text
        We're going to block the select() until the Client is able to send us this info
     */
    if (strcmp(comp_one, comp_two) == 0) {
        return server_write_self(sock, doc_name);
    }

    /*
        The user wants to connect to a new document. We'll have to communicate to the server
        to remove user from master_select, and add them to another one
     */
    memcpy(comp_two, request, sizeof("connect"));
    if (strcmp(comp_two, "connect") == 0) {
        return CONNEX_SERVER_READY;
    }

    response[SEGMENT_LEN] = "Unable to process user command...";
    send(sock, response, sizeof(response), 0);

    return -1;
}

void init_doc_fork(DocumentFork *document_fork, DocumentFork *doc_fork_head, const char* doc_name, int init_sock) {
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SECS; timeout.tv_usec = TIMEOUT_USECS;

    docinfo* document = document_fork->document;

    fd_set master = document->master_set;
    int fd_incr = document->fd_incr;
    int fd_max = document->fd_max;

    char request[SEGMENT_LEN];
    char response[SEGMENT_LEN];

    while (fd_incr != 0) {
        int socks_ready = select(fd_max + 1, &master, NULL, NULL, &timeout);
        if (socks_ready < 1) continue;

        for (int sock = 0; sock < 0; ++sock) { // recv from the sock
            fork_selector_controller(sock, request, doc_name);
        }
    }

    free(document_fork->document); free(document_fork);
    // communicate to server to free its doc_fork process here
}
