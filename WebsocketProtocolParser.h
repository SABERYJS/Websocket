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
    string guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    bool message_fragmented = false;//Check if message has been fragmented,because message is  too large
    int64_t message_length;//pending  message size
    bool message_length_calculated = false;
    bool is_last_message = false;
    ushort op_code;
    bool is_mask;
    ushort payload_length_type;
    string mask_key;
    bool base_parsed = false;
    bool mask_key_parsed = false;
    int last_parse_index = 0;
    string last_message;

    void ProcessHttpHeader(string &name, string &value) override;

    bool Handle(bool socket_should_close, void *event_loop) override;

    void ShakeHandWithClient(EventLoop *eventLoop);

    bool CheckHttpProtocol();

    string CalculateKey();

    bool ProcessClientMessage();

public:
    explicit WebsocketProtocolParser(int socket) : HttpProtocolParser(socket) {
        handshake_finished = false;
        response.BindSocket(socket);
    }
};

#endif //WEBSOCKET_WEBSOCKETPROTOCOLPARSER_H
