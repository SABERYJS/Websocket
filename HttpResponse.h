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

    void AddHeader(string &name, string &value, bool is_last);

    void AddStatusLine(string &protocol, int code, string &description);

};

#endif //WEBSOCKET_HTTPRESPONSE_H
