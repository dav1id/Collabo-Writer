//
// Created by David Ola on 2026-08-14.
//

#ifndef CLIENT_DOC_WRITER_H
#define CLIENT_DOC_WRITER_H

/**
    Append the contents of the document to the server using basic segmentation and f-functions

    @param sock File descriptor for the server socket
    @param doc_name Name of the document that will be updated
**/
void client_write_server(const int sock, const char* doc_name);


/**
    Other clients can call update() to update the server's .txt file. The server sends the updated
    text file to all the other clients. The clients call server_write_client locally() in their selector.

    @param sock File descriptor for the server socket
    @param doc_name Name of the document being updated
**/
enum CONNEX_STAGE server_write_client(const int sock, const char* doc_name);

/**
    Establishes a connection with the server, server deserializes the document if it exists.
    Creates one if it doesn't. Sends a response back to the client indicating that it's established a
    connection

 **/
void connect_docu(const int sock, const char* doc_name);

#endif //CLIENT_DOC_WRITER_H
