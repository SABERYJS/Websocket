//
// Created by Administrator on 2019/9/25 0025.
//

#include "public.h"
#include "Server.h"
#include "WebsocketServer.h"
#include "EchoServer.h"
#include "Config.h"
#include "Log.h"

Config global_config;
Log global_log;

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
            global_log.Open(global_config.GetLogPath().c_str());
            auto server = new EchoServer(global_config.GetPort(), INADDR_ANY, 10);
            server->Loop();
        } catch (const char *error) {
            cout << error << endl;
            exit(1);
        } catch (std::runtime_error error) {
            cout << error.what() << endl;
            exit(1);
        } catch (SystemCallException &exception) {
            cout << exception.what() << endl;
            exit(1);
        }

    } else {
        cout << "-c option is required" << endl;
        exit(1);
    }

}