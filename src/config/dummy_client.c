#include "../../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

/*
    File functions:
    1. fopen() - Creates a FILE struct and returns a pointer, or null if it wasn't able to find
    the file.
    2. fseek() - Sets the position address of the FILE struct. For example, since we're going
    to be segmenting our data and reading every 1024 byte, we'll increment the position address to start
    at the 1025th byte (similar to a program counter in a CPU).
    3. lseek() - Sets the position address of the file struct to the end of the file
    4. fread() - Reads from the file address listed in the FILE struct, and saves the bytes in the
    memory location listed by the user.
    5. fprintf() - Similar to printf, but allows the user to specify the destination
    6. ftell() - Returns the current file position
*/

int send_file_contents(int sock, FILE* fp) {
    if (fp == NULL) {
        printf("Error Number %d: %s\n", errno, strerror(errno));
        return -1;
    }

    long pos_addr = 0;

    fseek(fp, 0, SEEK_END);
    long max_addr = ftell(fp);
    fseek(fp, 0, SEEK_SET); // setting the program counter to the beginning

    printf("Max possible byte sent: %d\n", (int) max_addr);

    char request[SEGMENT_LEN];
    int incr = 0;
    while (pos_addr < max_addr) {
        fread(request, SEGMENT_LEN, sizeof(char), fp);
        pos_addr += SEGMENT_LEN;

        int bytes_sent = (int) send(sock, request, SEGMENT_LEN, 0);
        printf("Increment %d.... %d bytes sent... \n", incr++, bytes_sent);
    }

    return 0;
}

typedef struct addrinfo addrinfo;

int main() {
    /*
      getaddrinfo to generate struct addrinfos that best match our server and our hints. Can
      loop through until we get one that is ideal/exact.

      socket creation then connect the socket to the addrinfo. connect() assigns it an ephemeral
      (temporary port assigned by the OS since we don't need to care about the assigned port)

      call send_file_contents to send contents to the server
     */

    char* file_path = "../../src/config/in.txt"; // path relative to cmake
    FILE* fp = fopen(file_path, "rb");

    int peer_sock;

    addrinfo hints;
    memset(hints, 0 ,sizeof(hints));

    addrinfo *peer_addrinfo;

    int res = getaddrinfo("8080", "0", &hints, &peer_addrinfo);
    VERIFY_RSLT_RTRN(res, "addrinfo struct generation");

    res = socket(AI_FAMILY, AI_SOCKTYPE, AI_PROTOCOL);
    VERIFY_RSLT_RTRN(res, "socket creation");

    res = connect(peer_sock, peer_addrinfo->ai_addr, peer_addrinfo->ai_addrlen);
    VERIFY_RSLT_RTRN(res, "peering to server");

    printf("Server connection has been established - Initiating steps to send contents to server!");
    send_file_contents(peer_sock, fp);
}