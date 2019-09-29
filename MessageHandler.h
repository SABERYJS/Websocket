//
// Created by Administrator on 2019/9/29 0029.
//

#ifndef WEBSOCKET_MESSAGEHANDLER_H
#define WEBSOCKET_MESSAGEHANDLER_H

#include "public.h"

class MessageHandler {
public:
    virtual string ProcessMessage(string &pkg) = 0;
};

#endif //WEBSOCKET_MESSAGEHANDLER_H
