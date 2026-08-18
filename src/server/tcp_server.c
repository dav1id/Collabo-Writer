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

/*
       Can make the parent process be the only one that deals with the shared memory, by
       making the child process send instructions to stdout, then the parent process pick up instructions
       being sent to stdout to update the shared memory!

       Or, we use the stdout approach and make the selector be the one that controls the document_fork
       selector
*/
void set_listen_func(int listen_sock, DocumentFork* doc_fork_head) {
    sockaddr_storage temp_sock_addr;
    socklen_t storage_socklen = sizeof(temp_sock_addr);

    char request[SEGMENT_LEN];
    char* header; char* segment;

    while (1) {
        int remote_sock = accept(listen_sock, (sockaddr*) &temp_sock_addr, &storage_socklen);

        recv(listen_sock, request, SEGMENT_LEN, 0);
        VERIFY_RSLT(recv, "Attempting to receive connect message from new socket..");

        memcpy(header, request, sizeof("connect"));
        if (strcmp(header, "connect") != 0) continue;

        segment = &request[sizeof("connect")];

        DocumentFork* curr = doc_fork_head;

        while (curr->next != NULL) {
            if (strcmp(curr->doc_name, segment) == 0) {
                docinfo *document = curr->document;
                FD_SET(remote_sock, &document->master_set);
                break;
            }
            curr = curr->next;
        }

        //DocumentFork creation, and then init_fork_creation
        DocumentFork *document_fork = malloc(sizeof(DocumentFork));
        document_fork->doc_name = segment;

        docinfo *document = document_fork->document;
        FD_SET(remote_sock, &document->master_set);

        document->fd_incr += 1;
        document->fd_max = remote_sock;

        pid_t pid = fork();
        document_fork->pid = pid;
        curr->next = document_fork;

        if (pid == 0) {
            /*
                Pass in the same info, but we're going to assume that the document_forks and the master selector are going to be
                handled by the parent process with mmap()
             */
            printf("%s %s\n", "Child Process has been created for document", document_fork->doc_name);
            init_doc_fork(document_fork, doc_fork_head, segment);
            exit(0);
        }
    }
}

int main(int argc, char* argv[]) {
    addrinfo *hints;
    memset(&hints, 0, sizeof(addrinfo));

    hints->ai_family = AI_FAMILY;
    hints->ai_socktype = AI_SOCKTYPE;
    hints->ai_flags = AI_PASSIVE;

    addrinfo *bind_addr; // explanation of the double pointer for bind_addr
    int res = getaddrinfo("8080", NULL, hints, &bind_addr);
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