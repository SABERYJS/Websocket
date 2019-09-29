//
// Created by Administrator on 2019/9/25 0025.
//

#ifndef WEBSOCKET_WEBSOCKETSERVER_H
#define WEBSOCKET_WEBSOCKETSERVER_H

#include "public.h"
#include "Server.h"
#include "WebsocketProtocolParser.h"
#include "MessageHandler.h"

class WebsocketServer : public Server, public MessageHandler {
private:
    bool Handle(bool socket_should_close, void *event_loop) override;

public:
    WebsocketServer(int port, uint32_t ad, int back_log) : Server(port, ad, back_log) {}

    string ProcessMessage(string &pkg) override;
};

#endif //WEBSOCKET_WEBSOCKETSERVER_H
