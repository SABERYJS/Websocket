//
// Created by Administrator on 2019/9/23 0023.
//

#include "HttpResponse.h"


void HttpResponse::AddHeader(string &name, string &value, bool is_last = false) {
    internal_buffer.append(name + ":" + value + "\r\n");
    if (is_last) {
        //add header last time
        internal_buffer.append("\r\n");
    }
}

void HttpResponse::AddStatusLine(string protocol, int code, string description, bool is_last) {
    internal_buffer.append(protocol + " ").append(to_string(code) + " ").append(
            description + "\r\n" + (is_last ? "\r\n" : ""));
}

bool HttpResponse::Handle(bool socket_should_close, void *event_loop) {
    int ret;
    auto loop = (EventLoop *) event_loop;
    try_again:
    while ((ret = write(sock, internal_buffer.data(), internal_buffer.length())) > 0) {
        internal_buffer.erase(0, ret);//erase bytes that has been wroten
    }
    if (ret < 0) {
        if (errno == EINTR) {
            goto try_again;
        } else {
            //socket error,just close socket
            loop->CloseSocket(sock);
        }
    } else {
        if (close_after_write_finished) {
            close(sock);
        }
        return true;
    }
}