//
// Created by David Ola on 2026-08-14.
//

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

enum CONNEX_STAGE {
    CONNEX_SERVER_READY,
    CONNEX_SERVER_INCOMPL,
    APPEND_SERVER_COMPLETE
};

#endif //UTILS_H
