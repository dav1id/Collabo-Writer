#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/select.h>

#include <sys/errno.h>

#include "../../include/client_doc_controller.h"
#include "../../include/utils.h"

typedef struct addrinfo addrinfo;

int main(int argc, char* argv[]) {
    char* remote_addr = argv[1];
    char* port = argv[2];

    if (argc < 2) {
        remote_addr = "0";
        port = "8080";
    }

    addrinfo *peer_addr; addrinfo hints;
    memset(hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_TCP;

    int res = getaddrinfo(remote_addr, port, &hints, &peer_addr);
    VERIFY_RSLT_RTRN(res, "getaddrinfo");

    int host_sock = socket(peer_addr->ai_family, peer_addr->ai_socktype, peer_addr->ai_protocol);
    VERIFY_RSLT_RTRN(peer_addr, "listen");

    res = connect(host_sock, peer_addr->ai_addr, peer_addr->ai_addrlen);
    VERIFY_RSLT_RTRN(peer_addr, "connect");

    fd_set master; fd_set master_modif;
    FD_SET(&master, 0); //stdin
    FD_SET(&master, host_sock);

    char request[1024];
    char* response;

    while (1) {
        master_modif = master;
        int ready_socks = select(host_sock + 1, &master_modif, NULL, NULL, NULL);

        if (ready_socks > 0) {
            if (FD_ISSET(0, &master_modif) != 0) {
                if (fgets(request, 1024, stdin) != NULL) {
                    char* split;
                    if (memcpy(split, request, sizeof("connect")) == NULL) {
                        printf("%s\n", "Could not process document connect...");
                        continue;
                    }

                    if (strcmp(split, "connect") == 0) {

                    }

                    if (memcpy(split, request, sizeof("append"))) {
                        if (strcmp(split, "append") == 0) {
                            append_docu();
                        }
                    }
                }
            }
        }
    }
}