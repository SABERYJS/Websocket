//
// Created by Administrator on 2019/10/8 0008.
//

#ifndef WEBSOCKET_CONFIG_H
#define WEBSOCKET_CONFIG_H

#include "public.h"

class Config {
private:
    int port = 80;
    int process_num = 1;
    bool run_as_daemon = false;
    bool debug_open = true;
    string log_file_path;

    string config_filename;
    unordered_map<string, string> configs;


public:
    Config() {}

    explicit Config(string file) : config_filename(file) {}

    void Parse(string file = "");

    void Print();

    inline int GetPort() {
        return port;
    }

    inline int GetProcessNum() {
        return process_num;
    }

    inline bool CheckRunAsDaemon() {
        return run_as_daemon;
    }

    inline bool CheckDebugOpen() {
        return debug_open;
    }

    inline string GetLogPath() {
        return log_file_path;
    }

    /**
     * config file changed
     * **/
    void ReadConfigFileAgain();

};

#endif //WEBSOCKET_CONFIG_H
