cmake_minimum_required(VERSION 3.14)
project(websocket)

set(CMAKE_CXX_STANDARD 11)

add_executable(websocket main.cpp HttpProtocolParser.h public.h HttpProtocolParser.cpp utils.h WebsocketProtocolParser.cpp WebsocketProtocolParser.h HttpResponse.cpp HttpResponse.h Server.cpp Server.h SystemCallException.h EventHandler.h Event.h Event.cpp)