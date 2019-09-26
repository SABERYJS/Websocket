//
// Created by Administrator on 2019/9/24 0024.
//

#include "Event.h"
#include "EventCallback.h"

void EventLoop::AddEvent(int fd, int event_type, EventCallback *handler) {
    handlers[CreateMapKey(event_type, fd)] = handler;
    if (event_type == EVENT_READABLE) {
        original_readable.push_back(fd);
    } else if (event_type == EVENT_WRITEABLE) {
        original_writeable.push_back(fd);
    } else {
        original_error_happened.push_back(fd);
    }
}

void EventLoop::RemoveEvent(int fd, int event_type) {
    handlers.erase(CreateMapKey(event_type, fd));
    if (event_type == EVENT_READABLE) {
        original_readable.remove(fd);
    } else if (event_type == EVENT_WRITEABLE) {
        original_writeable.remove(fd);
    } else {
        original_error_happened.remove(fd);
    }
}

void EventLoop::ResetFdSet() {
    FD_ZERO(&readable);
    FD_ZERO(&writeable);
    FD_ZERO(&error_happened);
}

void EventLoop::ReInitFdSet() {
    ResetFdSet();
    ResetInitListenFdSet(&readable, EVENT_READABLE);
    ResetInitListenFdSet(&writeable, EVENT_WRITEABLE);
    ResetInitListenFdSet(&error_happened, EVENT_ERROR);
}

void EventLoop::ResetInitListenFdSet(fd_set *set, int event_type) {
    list<int> src;
    if (event_type == EVENT_READABLE) {
        src = original_readable;
    } else if (event_type == EVENT_WRITEABLE) {
        src = original_writeable;
    } else {
        src = original_error_happened;
    }
    auto is = src.begin();
    auto ie = src.end();
    while (is != ie) {
        FD_SET(*is, set);
        ++is;
    }
}

void EventLoop::Select() {
    int i,ret;
    while (true) {
        ReInitFdSet();
        try_again:
        if ((ret=select(FD_SETSIZE, &readable, &writeable, &error_happened, nullptr)) < 0) {
            if (errno == EINTR) {
                goto try_again;
            } else {
                cout<<strerror(errno)<<endl;
                throw SystemCallException(strerror(errno));
            }
        } else {
            for (i = 0; i < FD_SETSIZE; i++) {
                if (FD_ISSET(i, &readable)) {
                    handlers[CreateMapKey(EVENT_READABLE, i)]->Handle(
                            socket_should_close.find(i) != socket_should_close.end(),
                            this);
                }
                if (FD_ISSET(i, &writeable)) {
                    handlers[CreateMapKey(EVENT_WRITEABLE, i)]->Handle(
                            socket_should_close.find(i) != socket_should_close.end(),
                            this);
                }
                if (FD_ISSET(i, &error_happened)) {
                    handlers[CreateMapKey(EVENT_ERROR, i)]->Handle(
                            socket_should_close.find(i) != socket_should_close.end(),
                            this);
                }
            }
            //iterate finished
        }
    }
}