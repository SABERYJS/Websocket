//
// Created by Administrator on 2019/9/23 0023.
//

#include "WebsocketProtocolParser.h"

void WebsocketProtocolParser::ProcessHttpHeader(string &name, string &value) {
    value.erase(0, value.find_first_not_of(" "));//trim left space
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
    } else if ("Host" == name) {
        header_host = value;
    } else if ("Origin" == name) {
        header_origin = value;
    }
}


bool WebsocketProtocolParser::Handle(bool socket_should_close, void *event_loop) {
    if (this->event_loop == nullptr) {
        this->event_loop = (EventLoop *) event_loop;
    }
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
        ProcessClientMessage();
    }
}

/**
 *  0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+
 * **/
bool WebsocketProtocolParser::ProcessClientMessage() {
    ReadFromSocket();
    if (socket_closed) {
        event_loop->CloseSocket(socket);
        return false;
    }
    //firstly check if we received  full message
    size_t data_len = internal_buffer.length();
    if (!message_length_calculated) {
        if (data_len > 2) {
            short t = internal_buffer.at(1) & 0x7F;
            if (t >= 0 && t <= 125) {
                //no extended payload length
                message_length = t;
                message_length_calculated = true;
                payload_length_type = 1;
            } else if (t == 126) {
                if (data_len < 4) {
                    return true;
                } else {
                    message_length = ntohl(stoi(internal_buffer.substr(2, 2)));
                    message_length_calculated = true;
                    payload_length_type = 2;
                }
            } else {
                if (data_len < 10) {
                    return true;
                } else {
                    message_length = ntohll(stoll(internal_buffer.substr(2, 8)));
                    message_length_calculated = true;
                    payload_length_type = 3;
                }
            }
        } else {
            return true;
        }
    }
    if (!base_parsed && data_len >= 2) {
        base_parsed = true;
        unsigned char c = internal_buffer.at(0);
        //check FIN bit
        is_last_message = ((c & 0x80) >> 7) > 0;
        //check RSV1,RSV2 and RSV3,they must be 0
        if (((c & 0x40) >> 6) != 0 || ((c & 0x20) >> 5) != 0 || ((c & 0x10) >> 4) != 0) {
            return false;
        }
        op_code = c & 0x0F;
        if ((op_code >= 3 && op_code <= 7) || (op_code >= 11 && op_code <= 15)) {
            return false;
        }
        is_mask = (c & 0x01) != 0;
        if (!is_mask) {
            return false;
        }
    }
    if (message_length_calculated && !mask_key_parsed) {
        //check mask key
        int mask_key_offset = (payload_length_type == 1 ? 0 : (payload_length_type == 2 ? 2 : 8)) + 2;
        if ((data_len - mask_key_offset) < 4) {
            return true;
        } else {
            mask_key_parsed = true;
            mask_key = internal_buffer.substr(mask_key_offset, 4);
            //erase metadata
            internal_buffer.erase(0, mask_key_offset + 4);
        }
    }
    if (mask_key_parsed && (data_len = internal_buffer.length()) > 0) {
        //metadata parse finished,start parse content;
        int i = 0;
        char c;
        while (i <= (data_len - 1)) {
            c = internal_buffer.at(i);
            last_message.append(1, (char) (c ^ (mask_key[last_parse_index % 4])));
            i++;
            ++last_parse_index;
        }
        if (last_parse_index == message_length) {
            //current message parse finished,so reset some flag
            message_length_calculated = false;
            mask_key_parsed = false;
            base_parsed = false;
            last_parse_index = 0;
            if (is_last_message) {
                //message receive finished
                cout << last_message << endl;
                if (!is_handler_switched) {
                    event_loop->ModifyEvent(socket, EVENT_WRITEABLE, &websocket_response);
                    is_handler_switched = true;
                }
                ProxyMessage();
            }
        }
    }
}

void WebsocketProtocolParser::ProxyMessage() {
    string res = message_handler->ProcessMessage(last_message);
    SendMessageToClient(res);
}

void WebsocketProtocolParser::SendMessageToClient(string &resp) {
    websocket_response.Write(1, resp, true);
}

void WebsocketProtocolParser::ShakeHandWithClient(EventLoop *eventLoop) {
    if (handshake_finished) {
        return;
    } else {
        eventLoop->AddEvent(socket, EVENT_WRITEABLE, &response);
        if (!CheckHttpProtocol()) {
            //check failed
            response.AddStatusLine(http_version, 404, "Not Found", true);
            response.CloseSocketAfterWriteFinished();
        } else {
            //validate finished
            response.AddStatusLine(http_version, 101, "Switching Protocols");
            response.AddHeader("Upgrade", "websocket", false);
            response.AddHeader("Connection", "Upgrade", false);
            response.AddHeader("Sec-WebSocket-Accept", CalculateKey(), false);
            if (header_sec_webSocket_protocol.length() > 0) {
                response.AddHeader("Sec-WebSocket-Protocol", "chat", true);
            } else {
                response.HeaderSendFinished();
            }
            handshake_finished = true;//handshake finished
            internal_buffer.clear();//clear any data in the  internal buffer
        }
    }
}

string WebsocketProtocolParser::CalculateKey() {
    unsigned char buf[SHA_DIGEST_LENGTH] = {0};
    header_sec_webSocket_key.append(guid);
    SHA1(reinterpret_cast<const unsigned char *>(header_sec_webSocket_key.data()), header_sec_webSocket_key.length(),
         buf);
    return base64_encode(buf, sizeof(buf));
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
    if (stof(http_version.substr(http_version.find_first_of('/') + 1)) < 1.1) {
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
    if (header_sec_webSocket_protocol.length() > 0) {
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
    return true;
}