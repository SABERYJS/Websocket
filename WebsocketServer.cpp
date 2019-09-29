//
// Created by Administrator on 2019/9/25 0025.
//

#include "WebsocketServer.h"


bool WebsocketServer::Handle(bool socket_should_close, void *event_loop) {
    Server::Handle(socket_should_close, event_loop);
    auto parser = new WebsocketProtocolParser(client_connected, this);
    parser->BindEventSystem(&this->event_loop);
    this->event_loop.AddEvent(client_connected, EVENT_READABLE, parser);
}

string WebsocketServer::ProcessMessage(string &pkg) {
    throw "ProcessMessage Must Be Implemented";
}