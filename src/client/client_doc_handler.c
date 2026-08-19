#include "../../include/client_doc_controller.h"
#include "../../include/utils.h"

#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>

#include <string.h>
#include <sys/errno.h>

void connect_docu(int server_sock,  const char* doc_name) {
    char request[SEGMENT_LEN] = "connect ";

    enum CONNEX_STAGE *stage;


    snprintf(request, SEGMENT_LEN, doc_name);

    int res = send(server_sock, request, SEGMENT_LEN, 0);
    VERIFY_RSLT(res, "Client sending request to connect to the server");

    res = recv(server_sock, stage, SEGMENT_LEN, 0);
    VERIFY_RSLT(res, "Requesting server response on client connection...")

    // look into writing an enum over the network to finish connect_docu
}

void append_docu() {

}