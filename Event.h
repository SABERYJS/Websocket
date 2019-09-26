//
// Created by Administrator on 2019/9/24 0024.
//

#ifndef WEBSOCKET_EVENT_H
#define WEBSOCKET_EVENT_H

#include "public.h"
#include "EventCallback.h"
#include <list>
#include <set>
#include "SystemCallException.h"
#include "Debug.h"

using namespace std;

class EventLoop {
private:
    fd_set readable;
    fd_set writeable;
    fd_set error_happened;
    list<int> original_readable;
    list<int> original_writeable;
    list<int> original_error_happened;
    unordered_map<string, EventCallback *> handlers;
    set<int> socket_should_close;
    unordered_map<string, Debug *> debugs;

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

    void AddEvent(int fd, int event_type, EventCallback *handler);

    void RemoveEvent(int fd, int event_type);

    void Select();

    void CloseSocket(int fd) {
        RemoveEvent(fd, EVENT_READABLE);
        RemoveEvent(fd, EVENT_WRITEABLE);
        FD_CLR(fd, &readable);//clear readable fd set
        FD_CLR(fd, &writeable);//clear writeable fd set
        FD_CLR(fd, &error_happened);//clear socket error
        close(fd);
    }
};

#endif //WEBSOCKET_EVENT_H
