//
// Created by David Ola on 2026-08-14.
//
#pragma once
#include <stdio.h>

#ifndef UTILS_H
#define UTILS_H

#define AI_FAMILY AF_INET
#define AI_SOCKTYPE SOCK_STREAM
#define AI_PROTOCOL IPPROTO_TCP

#define SEGMENT_LEN 1024
#define TIMEOUT_SECS 7
#define TIMEOUT_USECS 0

#define VERIFY_RSLT_RTRN(res, inst){ \
    if(res < 0){\
        printf("%s failed - %d: %s", inst, errno, strerror(errno));\
        return -1; \
    }\
} \

#define VERIFY_RSLT(res, inst){ \
    if(res < 0){\
        printf("%s failed - %d: %s", inst, errno, strerror(errno));\
    }\
} \


typedef enum CONNEX_STAGE {
    CONNEX_SERVER_READY,
    CONNEX_SERVER_INCOMPL,
    UPDATE_COMPL,
    UPDATE_INCOMPL
} CONNEX_STAGE;

void write_with_offset(int max_offset, int sock, FILE *fp);
void recv_with_offset(int max_offset, int sock, FILE *fp);

#endif //UTILS_H
