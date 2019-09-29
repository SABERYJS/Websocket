//
// Created by Administrator on 2019/9/29 0029.
//

#include "WebsocketResponse.h"

bool WebsocketResponse::Handle(bool socket_should_close, void *event_loop) {
    int ret;
    EventLoop *loop = (EventLoop *) event_loop;
    try_again:
    while ((ret = write(sock, internal_buffer.data(), internal_buffer.length())) > 0) {
        internal_buffer.erase(0, ret);
    }
    if (ret < 0) {
        if (errno == EINTR) {
            goto try_again;
        } else {
            loop->CloseSocket(sock);
        }
    }
}

void WebsocketResponse::WriteRaw(int op_code, string to_write, bool is_last) {
}

/**
 * failed: A server must not mask any frames that it sends to the client.
 * **/
void WebsocketResponse::Write(int op_code, string to_write = nullptr, bool is_last = true) {
    size_t len = to_write.length();
    if (len == 0) {
        return;
    }
    if (op_code == 1) {
        //write text frame
        char c = 0;//first byte
        if (is_last) {
            c = c | 0x80;
        }
        c = c | op_code;//operation code
        internal_buffer.append(1, c);
        unsigned char b_2 = 0;
        //fill length
        if (len > 0 && len <= 125) {
            internal_buffer.append(1, (b_2 | len));
        } else if (len <= 65535) {
            internal_buffer.append(1, (b_2 | 126));
            internal_buffer.append(to_string(htons(len)));
        } else {
            internal_buffer.append(1, (b_2 | 127));
            internal_buffer.append(to_string(htonll(len)));
        }
        internal_buffer.append(to_write);
    }
}