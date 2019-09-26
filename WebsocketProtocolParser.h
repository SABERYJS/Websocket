//
// Created by Administrator on 2019/9/23 0023.
//

#ifndef WEBSOCKET_WEBSOCKETPROTOCOLPARSER_H
#define WEBSOCKET_WEBSOCKETPROTOCOLPARSER_H

#include "HttpProtocolParser.h"
#include "EventCallback.h"
#include "HttpResponse.h"

class WebsocketProtocolParser : public HttpProtocolParser, public EventCallback {
protected:
    string header_upgrade;
    string header_connection;
    string header_sec_webSocket_key;
    string header_sec_webSocket_protocol;
    string header_sec_webSocket_version;
    string header_host;
    string header_origin;
    bool handshake_finished;//check if handshake is finished
    HttpResponse response;//used for  handshake with client

    void ProcessHttpHeader(string &name, string &value) override;

    bool Handle(bool socket_should_close, void *event_loop) override;

    void ShakeHandWithClient(EventLoop *eventLoop);

    bool CheckHttpProtocol();

public:
    explicit WebsocketProtocolParser(int socket) : HttpProtocolParser(socket) {
        handshake_finished = false;
        response.BindSocket(socket);
    }
};

#endif //WEBSOCKET_WEBSOCKETPROTOCOLPARSER_H
