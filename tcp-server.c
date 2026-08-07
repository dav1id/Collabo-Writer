#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/errno.h>
#include <string.h>

#include <sys/select.h>
#include <unistd.h>

/*
    Need to create a sequence diagram to draw out how we're going to communicate with the remote
    host/more system design concepts of this kind of communication.
    1. Client is going to need to request to load a specific text, and then save it and its contents
    back to the server
    2. Multiple clients can edit a pdf and save the contents to the server -> No concurrency yet!

    A user is going to connect to the server. The server sends a message asking the client which
    text_

    1. Client connects to the server. We can use ncurses to create a descriptive terminal UI (Just to make the
    C project cleaner)
    2. Client can ask for three commands:
        a.) create name_file.txt
        b.) edit name_file.txt
        c.) save name_file.txt
    3. Server is going to run selector class to cycle through the create, edit, or save options. Server creates
    a fork for each .txt file.
*/

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

struct DocumentFork DocumentFork {
    int docID;
    const char* docName;
    pid_t pid;

    struct Document document{};
    struct DocumentFork *next;
};

struct Document {
        fd_set master; // Sockets that have been
};

typedef struct DocumentFork DocumentFork;
typedef struct Document Document;

/**
    Create the fork that is going to handle all the recvfroms from the different remote sockets connected
    to it. Creates document_fork and Document structs for book-keeping. Inside the fork() it's going
    to loop through its selector() and handle the recvfroms + conflicts when remote sockets call
    updte name.txt
**/
void* initForkCreation(const int doc_id, const char* doc_name) {
    pid_t pid = fork();
    DocumentFork *document_fork = malloc(sizeof(document_fork));

    document_fork->docID = doc_id;
    document_fork->docName = doc_name;
    document_fork->pid = pid;

    fd_set doc_editor_set;

    Document currDocument = document_fork->document;


    return document_fork;
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

    fd_set master; fd_set master_modif;
    FD_ZERO(&master); FD_ZERO(&master_modif);
    FD_SET(listen_sock, &master); FD_SET(stdin, &master);

    int max_sock = listen_sock;
    int ext = 0;

    sockaddr_storage temp_sock_addr;
    socklen_t socklen_storage = sizeof(temp_sock_addr);

    char request[SEGMENT_LEN];

    while (ext == 0) {
        master_modif = master;
        int num_socks = select(max_sock + 1, &master_modif, NULL, NULL, NULL);

        if (FD_ISSET(listen_sock, &master_modif)) {
            int new_sock = accept(listen_sock, (sockaddr*) &temp_sock_addr, &socklen_storage);

            char remote_name_addr[64];
            res = getnameinfo((sockaddr*) &temp_sock_addr, socklen_storage, remote_name_addr, sizeof(remote_name_addr), 0, 0, 0);
            VERIFY_RSLT(res, "getnameinfo after client socket has been accepted");
            FD_SET(new_sock, &master);

            printf("New address accepted by Server - %.*s", (int) sizeof(remote_name_addr), remote_name_addr);
        }

        if (num_socks > 0) {
            for (int sock_iter = listen_sock + 1; sock_iter < max_sock + 1; sock_iter++) {
                if (FD_ISSET(sock_iter, &master_modif)) {
                    /*
                        Call recvfrom() to know that the client is not currently occupied
                     */
                    recv(sock_iter, request, SEGMENT_LEN, 0);

                    // strcmp to make sure that we are calling recv only on create, before we delegate the recv to the fork we created
                    char concat_command[6];

                    for (int i = 0; i < 6; ++i)
                       concat_command[i] = request[i];

                    if (strcmp(concat_command, "create") != 0 ) {
                        /*
                            Each socket is going to communicate with the doc using a child process we call with fork.
                            In case the socket was not removed from the master FD_SET and assigned to a seperate fork.

                            Check if .txt file doesn't already exist on our own end
                         */

                        char* response = "Unable to receive your request as a seperate process for your document has not been created";
                        send(sock_iter, response, strlen(response), 0);
                        continue;

                    }

                    initForkCreation();
                    FD_CLR(sock_iter, &master);
                }
            }
        }
    }
}