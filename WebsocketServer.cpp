//
// Created by Administrator on 2019/9/25 0025.
//

#include "WebsocketServer.h"


bool WebsocketServer::Handle(bool socket_should_close, void *event_loop) {
    Server::Handle(socket_should_close, event_loop);
    this->event_loop.AddEvent(client_connected, EVENT_READABLE, new WebsocketProtocolParser(client_connected));
}