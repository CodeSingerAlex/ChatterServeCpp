#include <iostream>
#include "chatserver.hpp"
#include "chatservice.hpp"
#include "signal.h"
using namespace std;

// 服务器异常退出时，重置用户状态
void resetHandler(int) {
    ChatService::instance()->reset();
    exit(-1);
}
int main(int argc, char **argv) {
    if (argc < 3) {
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }

    char *ip = argv[1];

    uint16_t port = atoi(argv[2]);

    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress listenAddr(ip, port);
    ChatServer server(&loop, listenAddr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}