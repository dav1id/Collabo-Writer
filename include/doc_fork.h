//
// Created by David Ola on 2026-08-15.
//
#ifndef DOC_FORK_H
#define DOC_FORK_H

#include <unistd.h>
#include "../../include/utils.h"

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
    Server responds to the client twice using this function. One on a sucessful connection to the document fork,
    and another when the client's text has been sucessfuly sent over the network.
    @param sock Remote/client socket
    @param stage Stage of server and client handshake
 **/
void server_response_connection(const int sock, enum CONNEX_STAGE stage);

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
    Append a socket to the document fork's selector. The document fork's selector is going to be in
    shared memory.
**/
void append_selector_fork(docinfo *document_fork);

/**
    Update the contents of the document. Creates the document if it doesn't exist, or deserialise the document if
    its the first time calling update_doc. Uses init_handshake communicate with the client as it appends the contents
    to the server text file
**/
enum CONNEX_STAGE server_write_self(const char* doc_name);

/**
    Update the doc for every client.
**/
enum CONNEX_STAGE server_write_clients(const char* doc_name, int sock_list[]);


#endif //DOC_FORK_H
