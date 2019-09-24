//
// Created by Administrator on 2019/9/24 0024.
//

#ifndef WEBSOCKET_EVENTHANDLER_H
#define WEBSOCKET_EVENTHANDLER_H

#include "public.h"
#include "Event.h"
#include <set>

using namespace std;

#define EVENT_READABLE 1
#define EVENT_WRITEABLE 2
#define EVENT_ERROR 3

//event handler base class,all event handler must implement the interface
class EventHandler {
private:
    void *data;
public:
    explicit EventHandler(void *data) {
        this->data = data;
    }

    virtual bool Handle(bool socket_should_close, void *event_loop) = 0;

    void *GetData() {
        return data;
    }
};

#endif //WEBSOCKET_EVENTHANDLER_H
