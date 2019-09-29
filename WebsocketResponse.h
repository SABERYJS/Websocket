//
// Created by Administrator on 2019/9/29 0029.
//

#ifndef WEBSOCKET_WEBSOCKETRESPONSE_H
#define WEBSOCKET_WEBSOCKETRESPONSE_H

#include "WebsocketResponse.h"
#include "EventCallback.h"
#include "Event.h"

class WebsocketResponse : public EventCallback {
private:
    string internal_buffer;
    Socket_t sock;
public:
    bool Handle(bool socket_should_close, void *event_loop) override;

    void BindSocket(Socket_t socket) {
        sock = socket;
    }

    void Write(int op_code, string to_write, bool is_last);

    void WriteRaw(int op_code, string to_write, bool is_last);
};

#endif //WEBSOCKET_WEBSOCKETRESPONSE_H
