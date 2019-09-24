//
// Created by Administrator on 2019/9/24 0024.
//

#ifndef WEBSOCKET_SYSTEMCALLEXCEPTION_H
#define WEBSOCKET_SYSTEMCALLEXCEPTION_H

#include "public.h"

class SystemCallException : public std::exception {
private:
    char *msg;
public:
    explicit SystemCallException(char *msg) {
        this->msg = msg;
    }

    const char *what() const noexcept override {
        return msg;
    }
};

#endif //WEBSOCKET_SYSTEMCALLEXCEPTION_H
