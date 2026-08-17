#include "../../include/doc_fork.h"
#include "../../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/errno.h>
#include <string.h>

#include <sys/select.h>

void init_doc_fork(DocumentFork *document_fork, DocumentFork *doc_fork_head, const char* doc_name) {
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
            int bytes_recv = (int) recv(sock, request, SEGMENT_LEN, 0);

            char comp_one[64];
            snprintf(comp_one, sizeof(comp_one), "update %s", doc_name);
            char* comp_two  = &request[strlen(comp_one)];

            /*
                The user wants to update the document. Here we're going to send a message to the client
                saying that we're ready to receive the segment portions of the text
                We're going to block the select() until the Client is able to send us this info
             */
            if (strcmp(comp_one, comp_two) == 0) {
                continue;
            }

            /*
                The user wants to connect to a new document. We'll have to communicate to the server
                to remove user from master_select, and add them to another one
             */
            memcpy(comp_two, request, sizeof("connect"));
            if (strcmp(comp_two, "connect") == 0) {
                continue;
            }

            response[SEGMENT_LEN] = "Unable to process user command...";
            send(sock, response, sizeof(response), 0);
        }
    }

    free(document_fork->document); free(document_fork);
    // communicate to server to free its doc_fork process here
}
