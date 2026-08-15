#include "../../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
*/

int main() {
    char* file_path = "../config_files/client_in.txt";
    FILE* fp = fopen(file_path, "rb");

    if (fopen(file_path, "rb") == NULL)
        return 1;

    char buffer[SEGMENT_LEN];
    int program_counter = 1;
    int final_addr = lseek();




}