//
// Created by Administrator on 2019/9/25 0025.
//

#include "public.h"
#include "Server.h"
#include "WebsocketServer.h"

int main() {
     auto server = new WebsocketServer(80, INADDR_ANY, 10);
     server->Loop();
}