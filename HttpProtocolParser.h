//
// Created by Administrator on 2019/9/23 0023.
//

#ifndef WEBSOCKET_HTTPPROTOCOLPARSER_H
#define WEBSOCKET_HTTPPROTOCOLPARSER_H

#include "public.h"
#include "SystemCallException.h"

class HttpProtocolParser {
protected:
    bool is_parse_finished;//whether http request header has been parse finished
    bool is_request_line_parse_finished;//whether  http request line has been parse finished
protected:
    unordered_map<string, string> headers;//http headers
    int socket;//bind socket
    string internal_buffer;//internal buffer for receive content from client
    string method;//request method
    string uri;//request uri
    string http_version;//request http version
    int next_search_pos;
    bool socket_closed;

    void InitStatus() {
        is_parse_finished = false;
        is_request_line_parse_finished = false;
        socket = 0;
        headers.clear();//clear parsed headers
        internal_buffer.clear();//clear internal buffer
        next_search_pos = 0;
        socket_closed = false;
    }

    void ParseRequestLine(int pos);

    virtual void ProcessHttpHeader(string &name, string &value) = 0;

    size_t ReadFromSocket();

    void PrintHeaders();

public:
    HttpProtocolParser() {
        InitStatus();
    }

    explicit HttpProtocolParser(int socket) {
        InitStatus();
        this->socket = socket;
    }

    void SetSocket(int fd) {
        socket = fd;
        InitStatus();
    }

    void Parse();
};

#endif //WEBSOCKET_HTTPPROTOCOLPARSER_H
