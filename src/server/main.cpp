#include "chatserver.hpp"
#include "chatservice.hpp"
#include "signal.h"
using namespace std;

// 服务器异常退出时，重置用户状态
void resetHandler(int) {
    ChatService::instance()->reset();
    exit(-1);
}
int main() {
    signal(SIGINT, resetHandler);
    EventLoop loop;
    InetAddress listenAddr("127.0.0.1", 6000);
    ChatServer server(&loop, listenAddr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}