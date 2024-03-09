#include "chatserver.hpp"

using namespace std;

int main() {
    EventLoop loop;
    InetAddress listenAddr("127.0.0.1", 6000);
    ChatServer server(&loop, listenAddr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}