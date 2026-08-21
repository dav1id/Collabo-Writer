//
// Created by David Ola on 2026-08-14.
//

#ifndef CLIENT_DOC_WRITER_H
#define CLIENT_DOC_WRITER_H

/**
    Append the contents of the document to the server using basic segmentation and f-functions
**/
void client_write_server(int sock, const char* doc_name);


void server_write_client(int sock, const char* doc_name);

/**
    Establishes a connection with the server, server deserializes the document if it exists.
    Creates one if it doesn't. Sends a response back to the client indicating that it's established a
    connection
 **/
void connect_docu(int sock, const char* doc_name);

#endif //CLIENT_DOC_WRITER_H
