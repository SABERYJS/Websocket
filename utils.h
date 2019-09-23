//
// Created by Administrator on 2019/9/23 0023.
//

#ifndef WEBSOCKET_UTILS_H
#define WEBSOCKET_UTILS_H

#include "public.h"

vector<string> split(const string &str, const string &pattern) {
    vector<string> res;
    if (str == "")
        return res;
    //在字符串末尾也加入分隔符，方便截取最后一段
    string strs = str + pattern;
    size_t pos = strs.find(pattern);

    while (pos != strs.npos) {
        string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + 1, strs.size());
        pos = strs.find(pattern);
    }

    return res;
}

#endif //WEBSOCKET_UTILS_H
