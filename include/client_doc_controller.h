//
// Created by David Ola on 2026-08-14.
//

#ifndef CLIENT_DOC_WRITER_H
#define CLIENT_DOC_WRITER_H

/**
    Append the contents of the document to the server using basic segmentation and f-functions
**/
void append_docu();

/**
    Establishes a connection with the server, server deserializes the document if it exists.
    Creates one if it doesn't. Sends a response back to the client indicating that it's established a
    connection
 **/
void connect_docu(int server_sock, const char* doc_name);

#endif //CLIENT_DOC_WRITER_H
