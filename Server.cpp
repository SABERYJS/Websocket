//
// Created by Administrator on 2019/9/24 0024.
//

#include "Server.h"
#include <poll.h>

void Server::CreateSocket() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw SystemCallException((char *) "Create Socket failed");
    }
}

void Server::Bind() {
    if (bind(sock, (sockaddr *) &addr, sock_len) < 0) {
        throw SystemCallException((char *) "Bind Socket Failed");
    }
}

void Server::Listen() {
    if (listen(sock, back_log) < 0) {
        throw SystemCallException((char *) "Listen Failed");
    }
}