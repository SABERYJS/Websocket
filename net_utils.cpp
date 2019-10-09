//
// Created by Administrator on 2019/10/9 0009.
//

#include "net_utils.h"

void SetSocketNonBlock(int sock) {
    fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
}

void SetSocketReuseAddr(int socket) {
    int yes = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
}