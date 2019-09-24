//
// Created by Administrator on 2019/9/23 0023.
//

#ifndef WEBSOCKET_PUBLIC_H
#define WEBSOCKET_PUBLIC_H

#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>

using namespace std;

#define Socket_t int


inline bool CharIsSpace(char c) {
    return c == ' ';
}

#endif //WEBSOCKET_PUBLIC_H
