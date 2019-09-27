//
// Created by Administrator on 2019/9/23 0023.
//

#ifndef WEBSOCKET_HTTPRESPONSE_H
#define WEBSOCKET_HTTPRESPONSE_H

#include "public.h"
#include "EventCallback.h"
#include "Event.h"

class HttpResponse : public EventCallback {
private:
    string internal_buffer;
    bool status_line_send;
    bool protocol_added;
    bool status_code_added;
    bool description_added;
    string http_protocol_version;
    unordered_map<string, string> headers;
    Socket_t sock;
    bool close_after_write_finished;

    void InitStatus() {
        status_line_send = false;
        protocol_added = false;
        status_code_added = false;
        description_added = false;
        headers.clear();
        close_after_write_finished = false;
    }

public:
    explicit HttpResponse(Socket_t socket) {
        sock = socket;
        InitStatus();
    }

    HttpResponse() {
        InitStatus();
    }

    void AddHeader(string name, string value, bool is_last);

    void AddStatusLine(string protocol, int code, string description, bool is_last = false);

    bool Handle(bool socket_should_close, void *event_loop) override;

    void BindSocket(Socket_t socket) {
        sock = socket;
    }

    void CloseSocketAfterWriteFinished() {
        close_after_write_finished = true;
    }

    void HeaderSendFinished() {
        internal_buffer.append("\r\n");
    }
};

#endif //WEBSOCKET_HTTPRESPONSE_H
