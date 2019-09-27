//
// Created by Administrator on 2019/9/27 0027.
//

#include "utils.h"

unsigned long long ntohll(unsigned long long val) {
    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
        return (((unsigned long long) htonl((int) ((val << 32) >> 32))) << 32) |
               (unsigned int) htonl((int) (val >> 32));
    } else if (__BYTE_ORDER == __BIG_ENDIAN) {
        return val;
    }
}

unsigned long long htonll(unsigned long long val) {
    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
        return (((unsigned long long) htonl((int) ((val << 32) >> 32))) << 32) |
               (unsigned int) htonl((int) (val >> 32));
    } else if (__BYTE_ORDER == __BIG_ENDIAN) {
        return val;
    }
}
