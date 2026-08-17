//
// Created by David Ola on 2026-08-15.
//
#ifndef DOC_FORK_H
#define DOC_FORK_H
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
    Controls the fd_set for the server. Server administers commands to document_fork, like
    init_fork_creation

    Connect() is going to connect the client to the document they want to begin to edit.
    We'll loop through our DocumentForks linked list (by the doc name) and create a process if
    a process to handle the document hasn't already been created. If it has, we'll have to add the
    client that wants to connect to the fd_read_set of the process. If it hasn't then we'll have
    to load the contents and then create the DocumentFork
 **/
void set_master_selector(int listen_sock);

/**
    Create the fork that is going to handle all the recvfroms from the different remote sockets connected
    to it. Creates document_fork and Document structs for bookkeeping. Inside the fork() it's going
    to loop through its selector() and handle the recvfroms + conflicts when remote sockets call
    update name.txt

    The fork is created and the socket that created it is assigned to its fd_read_set.
    (insert_function_name) here tells it
**/
void init_doc_fork(DocumentFork *document_fork, DocumentFork *doc_fork_head,  const char* doc_name);

#endif //DOC_FORK_H
