//
// Created by Administrator on 2019/9/23 0023.
//

#include "HttpResponse.h"


void HttpResponse::AddHeader(string &name, string &value, bool is_last = false){
    internal_buffer.append(name + ":" + value + "\r\n");
    if (is_last) {
        //add header last time
        internal_buffer.append("\r\n");
    }
}

void HttpResponse::AddStatusLine(string &protocol, int code, string &description){
    internal_buffer.append(protocol + " ").append(to_string(code) + " ").append(description + "\r\n");
}