#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/errno.h>
#include <string.h>

#include <sys/select.h>
#include <unistd.h>

#include "../../include/utils.h"
#include "../../include/doc_fork.h"

typedef struct addrinfo addrinfo;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_storage sockaddr_storage;

DocumentFork* doc_fork_head = NULL;

void set_master_selector(int listen_sock) {
    fd_set master; fd_set master_modif;
    FD_ZERO(&master); FD_ZERO(&master_modif);
    FD_SET(listen_sock, &master); FD_SET(0, &master); // add std_in

    int max_sock = listen_sock;

    sockaddr_storage temp_sock_addr;
    socklen_t socklen_storage = sizeof(temp_sock_addr);

    char request[SEGMENT_LEN]; char response[SEGMENT_LEN];

    while (1) {
        master_modif = master;
        int num_socks = select(max_sock + 1, &master_modif, NULL, NULL, NULL);

        if (FD_ISSET(listen_sock, &master_modif)) {
            int new_sock = accept(listen_sock, (sockaddr*) &temp_sock_addr, &socklen_storage);

            char remote_name_addr[64];
            int res = getnameinfo((sockaddr*) &temp_sock_addr, socklen_storage, remote_name_addr, sizeof(remote_name_addr), 0, 0, 0);
            VERIFY_RSLT(res, "getnameinfo after client socket has been accepted");
            FD_SET(new_sock, &master);

            printf("New address accepted by Server - %.*s", (int) sizeof(remote_name_addr), remote_name_addr);
        }

        if (num_socks > 0) {
            for (int sock = listen_sock + 1; sock < max_sock + 1; sock++) {
                if (FD_ISSET(sock, &master_modif)) {

                    recv(sock, request, SEGMENT_LEN, 0);
                    char cmp_one[64] = "connect";
                    char* cmp_two = &request[strlen(cmp_one)];

                    if (strcmp(cmp_one, cmp_two) == 0) {
                        char* doc_name = &request[sizeof("connect")];
                        int found = 0;
                        DocumentFork *curr = doc_fork_head;

                        while (curr != NULL && curr->next != NULL) {
                            if (curr->doc_name == doc_name) {
                                found = 1;
                                break;
                            }
                        }

                        if (found == 0) {
                            DocumentFork *document_fork = malloc(sizeof(DocumentFork));

                            init_fork_creation(doc_fork_head, document_fork, sock, doc_name);
                            FD_CLR(sock, &master);

                        } else {
                            /*
                                Communicate with the existing fork and add it to its selector
                             */
                        }

                        /*
                           Send a message saying we're ready to receive the contents (A 1),
                           and then serialize it in our server
                        */

                        response[SEGMENT_LEN] = "1 - Ready to receive contents";
                        send(sock, response, strlen(response), 0);

                        // Call recv here
                    }

                    response[SEGMENT_LEN] = "Unable to receive your request as a separate process for your document has not been created";
                    send(sock, response, strlen(response), 0);
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    addrinfo *hints;
    memset(&hints, 0, sizeof(addrinfo));

    hints->ai_family = AI_FAMILY;
    hints->ai_socktype = AI_SOCKTYPE;

    addrinfo *bind_addr; // explanation of the double pointer for bind_addr
    int res = getaddrinfo(0, "8080", hints, &bind_addr);
    VERIFY_RSLT_RTRN(res, "getaddrinfo");

    const int listen_sock = socket(AI_FAMILY, AI_SOCKTYPE, bind_addr->ai_protocol);
    VERIFY_RSLT_RTRN(listen_sock, "listen");

    res = bind(listen_sock, bind_addr->ai_addr, bind_addr->ai_addrlen);
    VERIFY_RSLT_RTRN(res, "bind");

    res = listen(listen_sock, 2);
    VERIFY_RSLT_RTRN(res, "listen");

    set_master_selector(listen_sock);

    return 1;
}