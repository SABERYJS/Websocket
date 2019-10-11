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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <openssl/sha.h>
#include "base64.h"
#include "utils.h"
#include "net_utils.h"
#include <signal.h>
#include <wait.h>

using namespace std;

#define Socket_t int


inline bool CharIsSpace(char c) {
    return c == ' ';
}

#endif //WEBSOCKET_PUBLIC_H
