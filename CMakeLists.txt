cmake_minimum_required(VERSION 3.14)
project(websocket)

set(CMAKE_CXX_STANDARD 11)

add_executable(websocket main.cpp HttpProtocolParser.h public.h HttpProtocolParser.cpp utils.h WebsocketProtocolParser.cpp WebsocketProtocolParser.h HttpResponse.cpp HttpResponse.h Server.cpp Server.h SystemCallException.h Event.h Event.cpp WebsocketServer.h WebsocketServer.cpp EventCallback.h Debug.h main.cpp base64.cpp base64.h utils.cpp EchoServer.cpp EchoServer.h MessageHandler.cpp MessageHandler.h WebsocketResponse.cpp WebsocketResponse.h Config.cpp Config.h)

target_link_libraries(websocket -lcrypto)