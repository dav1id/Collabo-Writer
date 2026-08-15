#include "../../include/server_doc_writer.h"
#include "../../include/doc_fork.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/errno.h>
#include <string.h>

#include <sys/select.h>
#include <unistd.h>


void init_fork_creation(DocumentFork *document_fork, DocumentFork *doc_fork_head, const int start_sock, const char* doc_name) {
    pid_t pid = fork();

    document_fork->doc_name = doc_name;
    document_fork->pid = pid;

    document_fork->document = malloc(sizeof(docinfo));
    docinfo* doc = document_fork->document;

    DocumentFork *curr = doc_fork_head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = document_fork;

    fd_set master;
    int max = start_sock;

    FD_ZERO(&master);
    doc->master_set = master;

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SECS; timeout.tv_usec = TIMEOUT_USECS;

    if (pid > 0) {
        char response[64];
        char request[SEGMENT_LEN];

        while (sizeof(master) != 0) {
            int socks_ready = select(max, &master, NULL, NULL, &timeout);

            if (socks_ready != 0) {
                /*
                    Assume that next message user sends is going to include in the document in segments,
                    which we're going to handle on the client side then go and handle on the server side!
                */

                for (int sock = 0; sock < (max+1); ++sock) {
                    int bytes_recv = (int) recv(sock, request, SEGMENT_LEN, 0); // for debugging bookeeping

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
                        The user wants to connect to a new document. We'll have to tell the user to use the
                        connect doc_name to switch to a different document
                     */
                    memcpy(comp_two, request, sizeof("connect"));
                    if (strcmp(comp_two, "connect") == 0) {
                        continue;
                    }

                    response[64] = "Unable to process user command...";
                    send(sock, response, sizeof(response), 0);
                }
            }
        }

        free(document_fork->document); free(document_fork);
    }
}