//
// Created by Administrator on 2019/10/8 0008.
//

#ifndef WEBSOCKET_CONFIG_H
#define WEBSOCKET_CONFIG_H

#include "public.h"

class Config {
private:
    int port;
    int process_num;
    bool run_as_daemon;
    string config_filename;
    unordered_map<string, string> configs;


public:
    Config() {}

    explicit Config(string file) : config_filename(file) {}

    void Parse(string file = "");

    void Print();
};

#endif //WEBSOCKET_CONFIG_H
