//
// Created by Administrator on 2019/9/23 0023.
//

#ifndef WEBSOCKET_HTTPRESPONSE_H
#define WEBSOCKET_HTTPRESPONSE_H

#include "public.h"

class HttpResponse {
private:
    string internal_buffer;
    bool status_line_send;
    bool protocol_added;
    bool status_code_added;
    bool description_added;
    string http_protocol_version;
    unordered_map<string, string> headers;
public:
    HttpResponse() {
        status_line_send = false;
        protocol_added = false;
        status_code_added = false;
        description_added = false;
        headers.clear();
    }

    void AddHeader(string &name, string &value, bool is_last = false) {
        internal_buffer.append(name + ":" + value + "\r\n");
        if (is_last) {
            //add header last time
            internal_buffer.append("\r\n");
        }
    }

    void AddStatusLine(string &protocol, int code, string &description) {
        internal_buffer.append(protocol + " ").append(to_string(code) + " ").append(description + "\r\n");
    }

};

#endif //WEBSOCKET_HTTPRESPONSE_H
