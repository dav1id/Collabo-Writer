#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/errno.h>
#include <string.h>

#include <sys/select.h>
#include <unistd.h>

typedef struct addrinfo addrinfo;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_storage sockaddr_storage;

#define VERIFY_RSLT_RTRN(res, inst){ \
    if(res < 0){\
        printf("%s failed| %d: %s", inst, errno, strerror(errno));\
        return -1; \
    }\
} \

#define VERIFY_RSLT(res, inst){ \
    if(res < 0){\
        printf("%s failed| %d: %s", inst, errno, strerror(errno));\
    }\
} \

#define AI_FAMILY AF_INET
#define AI_SOCKTYPE SOCK_STREAM

//Segmentation
#define SEGMENT_LEN 1024
#define TIMEOUT_SECS 7
#define TIMEOUT_USECS 0

typedef struct DocumentFork DocumentFork;
typedef struct docinfo docinfo;

struct DocumentFork DocumentFork {
    const char* doc_name;
    pid_t pid;

    docinfo *document;
    DocumentFork *next;
};

struct docinfo {
    fd_set master_set; // Sockets that have been
};

DocumentFork* doc_fork_head = NULL;


void update_document();

/**
    Create the fork that is going to handle all the recvfroms from the different remote sockets connected
    to it. Creates document_fork and Document structs for bookkeeping. Inside the fork() it's going
    to loop through its selector() and handle the recvfroms + conflicts when remote sockets call
    update name.txt

    The fork is created and the socket that created it is assigned to its fd_read_set.
    (insert_function_name) here tells it
**/
void initForkCreation(DocumentFork *document_fork, int start_sock, const char* doc_name) {
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
                    if (strcmp(comp_two, "create") == 0) {
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

void master_selector(int listen_sock) {
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


                    /*
                        Connect() is going to connect the client to the document they want to begin to edit.
                        We'll loop through our DocumentForks linked list (by the doc name) and create a process if
                        a process to handle the document hasn't already been created. If it has, we'll have to add the
                        client that wants to connect to the fd_read_set of the process. If it hasn't then we'll have
                        to load the contents and then create the DocumentFork
                    */

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

                            initForkCreation(document_fork, sock, doc_name);
                            FD_CLR(sock, &master);

                        } else {
                            /*
                                Communicate with the existing fork and add it to its selector
                             */
                        }

                        /*
                           Send a message saying we're ready to receive the contents,
                           and then serialize it in our server
                        */
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

    master_selector(listen_sock);

    return 1;
}