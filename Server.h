//
// Created by Administrator on 2019/9/24 0024.
//

#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include "public.h"
#include "SystemCallException.h"
#include "EventHandler.h"
#include "Event.h"

class Server {
private:
    int port;//server port
    Socket_t sock;//server listen socket
    sockaddr_in addr;
    socklen_t sock_len;
    int back_log;
    timeval time_out;
    EventLoop *loop;


    void CreateSocket();

public:
    explicit Server(int port, uint32_t ad, int back_log) {
        this->port = port;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(ad);
        sock_len = sizeof(addr);
        this->back_log = back_log;
        time_out.tv_sec = 0;
        time_out.tv_usec = 0;
    }


    void Bind();

    void Listen();

    void Loop();

};

#endif //WEBSOCKET_SERVER_H
