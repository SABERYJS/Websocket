//
// Created by Administrator on 2019/9/23 0023.
//

#include "WebsocketProtocolParser.h"

void WebsocketProtocolParser::ProcessHttpHeader(string &name, string &value) {
    if ("Upgrade" == name) {
        header_upgrade = value;
    } else if ("Connection" == name) {
        header_connection = value;
    } else if ("Sec-WebSocket-Key" == name) {
        header_sec_webSocket_key = value;
    } else if ("Sec-WebSocket-Protocol" == name) {
        header_sec_webSocket_protocol = value;
    } else if ("Sec-WebSocket-Version" == name) {
        header_sec_webSocket_version = value;
    } else if ("Host" == value) {
        header_host = value;
    } else if ("Origin" == value) {
        header_origin = value;
    }
}


bool WebsocketProtocolParser::Handle(bool socket_should_close, void *event_loop) {
    if (!handshake_finished) {
        //handshake not finished
        if (!is_parse_finished) {
            Parse();//parse http header
        }
        if (is_parse_finished) {
            //negotiation handshake with client
            ShakeHandWithClient((EventLoop *) event_loop);
        }
    } else {

    }
}

void WebsocketProtocolParser::ShakeHandWithClient(EventLoop *eventLoop) {
    if (handshake_finished) {
        return;
    } else {
        if (!CheckHttpProtocol()) {
            //check failed
            eventLoop->AddEvent(socket, EVENT_WRITEABLE, &response);
            response.AddStatusLine(http_version, 404, "Not Found", true);
            response.CloseSocketAfterWriteFinished();
        } else {
            //validate finished
        }
    }
}

bool WebsocketProtocolParser::CheckHttpProtocol() {
    size_t l = method.length();
    //check request method
    if (strncmp(method.data(), "GET", l) != 0) {
        return false;
    }
    //check Upgrade http header
    if (header_upgrade.length() == 0 || strncmp(header_upgrade.data(), "websocket", 9) != 0) {
        return false;
    }
    //check Connection http header
    if (header_connection.length() == 0 || strncmp(header_connection.data(), "Upgrade", 7) != 0) {
        return false;
    }
    //check Sec-WebSocket-Key,Host http header
    if (header_sec_webSocket_key.length() == 0 || header_host.length() == 0 || header_origin.length() == 0) {
        return false;
    }
    //check http version,must be greater than 1.1
    if (stof(http_version) < 1.1) {
        return false;
    }
    //check websocket version,must be 13
    if (header_sec_webSocket_version.length() == 0 || stoi(header_sec_webSocket_version) != 13) {
        return false;
    }
    //check websocket protocol
    int offset = 0;
    int pos;
    bool matched = false;
    while ((pos = header_sec_webSocket_protocol.find_first_of(',', offset)) != string::npos) {
        if (header_sec_webSocket_protocol.substr(offset, pos) == "chat") {
            //we only support char protocol
            matched = true;
            break;
        }
        offset = pos;
    }
    return matched;
}