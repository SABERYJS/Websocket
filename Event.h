//
// Created by Administrator on 2019/9/24 0024.
//

#ifndef WEBSOCKET_EVENT_H
#define WEBSOCKET_EVENT_H

#include "public.h"
#include "EventHandler.h"
#include <list>
#include <set>
#include  "SystemCallException.h"

class EventLoop {
private:
    fd_set readable;
    fd_set writeable;
    fd_set error_happened;
    list<int> original_readable;
    list<int> original_writeable;
    list<int> original_error_happened;
    unordered_map<string, EventHandler *> handlers;
    set<int> socket_should_close;

    string CreateMapKey(int event_type, int fd) {
        return to_string(fd) + ":" + to_string(event_type);
    }

    void ResetFdSet();

    void ReInitFdSet();

    void ResetInitListenFdSet(fd_set *set, int event_type);

public:
    EventLoop() {
        ResetFdSet();
        handlers.clear();
        original_readable.clear();
        original_writeable.clear();
        original_error_happened.clear();
    }

    void AddEvent(int fd, int event_type, EventHandler *handler);

    void RemoveEvent(int fd, int event_type);

    void Select();

    void CloseSocket(int fd) {
        socket_should_close.insert(fd);
    }
};

#endif //WEBSOCKET_EVENT_H
