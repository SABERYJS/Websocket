//
// Created by Administrator on 2019/9/23 0023.
//

#ifndef WEBSOCKET_WEBSOCKETPROTOCOLPARSER_H
#define WEBSOCKET_WEBSOCKETPROTOCOLPARSER_H

#include "HttpProtocolParser.h"

class WebsocketProtocolParser : public HttpProtocolParser {
protected:
    string header_upgrade;
    string header_connection;
    string header_sec_webSocket_key;
    string header_sec_webSocket_protocol;
    string header_sec_webSocket_version;

    void ProcessHttpHeader(string &name, string &value) override;
};

#endif //WEBSOCKET_WEBSOCKETPROTOCOLPARSER_H
