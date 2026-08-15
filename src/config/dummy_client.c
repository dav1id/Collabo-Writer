#include "../../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

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

int main() {
    char* cwd = getcwd(NULL, 0);
    printf("Working directory is: %s\n", cwd);
    free(cwd);

    char* file_path = "../../src/config/in.txt"; // path relative to cmake
    FILE* fp = fopen(file_path, "rb");

    if (fp == NULL) {
        printf("Error Number %d: %s\n", errno, strerror(errno));
        return -1;
    }

    char buffer[SEGMENT_LEN];
    int program_counter = 1;

    fseek(fp, 0, SEEK_END);
    long final_pos = ftell(fp);
    printf("%d\n", (int) final_pos);
}