//
// Created by Administrator on 2019/9/29 0029.
//

#ifndef WEBSOCKET_ECHOSERVER_H
#define WEBSOCKET_ECHOSERVER_H

#include "public.h"
#include "WebsocketServer.h"

class EchoServer : public WebsocketServer {
public:
    EchoServer(int port, uint32_t ad, int back_log):WebsocketServer(port,ad,back_log){}
    string ProcessMessage(string &pkg) override;
};

#endif //WEBSOCKET_ECHOSERVER_H
