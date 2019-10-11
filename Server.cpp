//
// Created by Administrator on 2019/9/24 0024.
//

#include "Server.h"
#include <poll.h>

void Server::CreateSocket() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    SetSocketNonBlock(sock);
    SetSocketReuseAddr(sock);
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


void Server::Loop() {
    CreateSocket();
    Bind();
    Listen();
}

void Server::WaitClient() {
    global_event_loop.AddEvent(sock, EVENT_READABLE, this);
    global_event_loop.Select();
}

bool Server::Handle(bool socket_should_close, void *global_event_loop) {
    //has new connection
    sockaddr_in s_ad;
    socklen_t l;
    client_connected = accept(sock, (sockaddr *) &s_ad, &l);
    if (client_connected < 0) {
        throw SystemCallException((char *) "Accept Called Failed");
    }
    return true;
}