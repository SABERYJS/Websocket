//
// Created by Administrator on 2019/9/25 0025.
//

#include "public.h"
#include "Server.h"
#include "WebsocketServer.h"
#include "EchoServer.h"
#include "Config.h"

Config global_config;

int main() {
    global_config.Parse("E:\\code\\websocket\\app.conf");
    global_config.Print();
    /*auto server = new EchoServer(80, INADDR_ANY, 10);
    server->Loop();*/
}