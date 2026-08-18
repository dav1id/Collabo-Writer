//
// Created by David Ola on 2026-08-15.
//
#ifndef DOC_FORK_H
#define DOC_FORK_H

#include <unistd.h>
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
    int fd_incr = 0; // number of fds in the set
    int fd_max;
};


/**
    Client sends a message to the server to connect. Server sends a message to the client letting it know
    that it's ready to receive its segment information. Client sends metadata of the .txt file it's about to send
    (Stage 2). Server lets the Client know that it's finished receiving the segments by returning the number of bytes
    received (Stage 3)
    @param sock Remote/client socket
    @param stage Stage of server and client handshake
 **/
void init_handshake(const int sock, const int stage);

/**
    Create the fork that is going to handle all the recvfroms from the different remote sockets connected
    to it. Creates document_fork and Document structs for bookkeeping. Inside the fork() it's going
    to loop through its selector() and handle the recvfroms + conflicts when remote sockets call
    update name.txt

    The fork is created and the socket that created it is assigned to its fd_read_set.
    (insert_function_name) here tells it
**/
void init_doc_fork(DocumentFork *document_fork, DocumentFork *doc_fork_head,  const char* doc_name);


/**
    Update the contents of the document. Creates the document if it doesn't exist, or deserialise the document if
    its the first time calling update_doc. Uses init_handshake communicate with the client as it appends the contents
    to the server text file
**/
void update_doc(const char* doc_name);

/**
 **/
void deserialise_doc(const char* doc_name);

void serialise_doc(const char* doc_name);

#endif //DOC_FORK_H
