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
    }
}