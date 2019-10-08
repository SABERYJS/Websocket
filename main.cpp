//
// Created by Administrator on 2019/9/25 0025.
//

#include "public.h"
#include "Server.h"
#include "WebsocketServer.h"
#include "EchoServer.h"
#include "Config.h"

Config global_config;

int main(int argc, char *argv[]) {
    int i = 0;
    bool config_file_specified = false;
    string config_file;
    for (; i < argc; i++) {
        if (strncmp(argv[i], "-c", 2) == 0 && i != (argc - 1)) {
            config_file_specified = true;
            config_file = argv[i + 1];
            break;
        }
    }
    if (config_file_specified) {
        try {
            global_config.Parse(config_file);
            auto server = new EchoServer(80, INADDR_ANY, 10);
            server->Loop();
        } catch (const char *error) {
            cout << error << endl;
            exit(1);
        }

    } else {
        cout << "-c option is required" << endl;
        exit(1);
    }

}