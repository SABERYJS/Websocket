//
// Created by Administrator on 2019/9/23 0023.
//

#include "HttpProtocolParser.h"


void HttpProtocolParser::Parse() {
    ReadFromSocket();//read from socket
    while (true) {
        if (!is_parse_finished) {
            int pos = internal_buffer.find_first_of('\r', next_search_pos);
            if (pos == string::npos) {
                //can not find '\r'
                return;
            } else if (pos == next_search_pos) {
                //header matched finished
                is_parse_finished = true;
            } else {
                if (!is_parse_finished) {
                    if (!is_request_line_parse_finished) {
                        ParseRequestLine(pos);
                        next_search_pos = pos + 2;
                    } else {
                        int colon_pos = internal_buffer.find_first_of(':', next_search_pos);//look colon position
                        string header_name = internal_buffer.substr(next_search_pos,
                                                                    (colon_pos - next_search_pos));
                        string header_value = internal_buffer.substr(
                                colon_pos + 1,
                                (pos - colon_pos - 1));
                        headers[header_name] = header_value;
                        next_search_pos = pos + 2;//exclude '\r\n'
                        ProcessHttpHeader(header_name, header_value);
                    }
                } else {
                    //ignore request content
                }
            }
        } else {
            //PrintHeaders();
            return;
        }
    }
}

void HttpProtocolParser::PrintHeaders() {
    auto ab = headers.begin();
    auto ae = headers.end();
    while (ab != ae) {
        cout << (*ab).first << endl;
        cout << (*ab).second << endl;
        ++ab;
    }
}

void HttpProtocolParser::ParseRequestLine(int pos) {
    bool method_parsed = false;
    bool url_parsed = false;
    bool match_start = false;
    bool is_last;
    int start = 0;
    char c;
    for (int i = 0; i < pos; i++) {
        c = internal_buffer[i];
        is_last = (i == (pos - 1));
        if (!match_start && !CharIsSpace(c)) {
            match_start = true;
            start = i;
        } else if ((match_start && CharIsSpace(c)) || (is_last && match_start)) {
            if ((match_start && CharIsSpace(c))) {
                //reset status
                match_start = false;
            }
            //part matched finished
            if (!method_parsed) {
                method_parsed = true;
                method = internal_buffer.substr(start, (i - start));
            } else if (!url_parsed) {
                url_parsed = true;
                uri = internal_buffer.substr(start, (i - start));
            } else {
                http_version = internal_buffer.substr(start, (i - start) + (is_last ? 1 : 0));
            }
        }
    }
    is_request_line_parse_finished = true;
}


size_t HttpProtocolParser::ReadFromSocket() {
    char temp[1024] = {0};
    int ret;
    size_t has_read = 0;
    while (true) {
        try_again:
        ret = read(socket, temp, 1024);
        if (ret < 0) {
            if (errno == EINTR) {
                goto try_again;
            } else {
                throw SystemCallException((char *) "Read from Socket failed");
            }
        } else if (ret == 0) {
            socket_closed = true;
            return has_read;
        } else {
            has_read += ret;
            internal_buffer.append(temp, ret);
            if (ret < 1024) {
                return has_read;
            }
        }
    }
}
