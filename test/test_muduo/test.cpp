#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>

using namespace std;
using namespace muduo;
using namespace muduo::net; 
using namespace std::placeholders;
  
/*
如何使用muduo开发网络程序
1. 组合TcpServer对象。
2. 创建EventLoop事件循环对象的指针。
3. 明确TcpSever和EventLoop需要什么参数，以便初始化。
4. 在当前服务器类的构造函数中，注册处理连接和读写事件的回调函数。
5. 设置合适的服务器端线程数量， 1个I/O线程， 3个woker线程。
*/
class ChatServer {
public:
    ChatServer(EventLoop* loop, // 事件循环
            const InetAddress& listenAddr, // IP + Port
            const string& nameArg) // 服务器的名字
            : _server(loop, listenAddr, nameArg)
            , _loop(loop)
    {
        // 给服务器注册用户创建和断开连接的回调
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

        // 给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

        // 设置服务器端的线程数量，muduo会自动分配，Reactor线程（负责分发的线程）和工作线程
        _server.setThreadNum(4);
    }

    // 开启事件循环
    void start() {
        _server.start();
    }
private:
    // 专门处理用户的连接创建和断开
    void onConnection(const TcpConnectionPtr &conn) {
        if(conn->connected()) {
            cout << conn->peerAddress().toIpPort() << "-> "
            << conn->localAddress().toIpPort() << " state: online" << endl;
        } else {
            cout << conn->peerAddress().toIpPort() << "-> "
            << conn->localAddress().toIpPort() << " state: offline" << endl;
            conn->shutdown(); // close();
            // _loop.quit();
        }
    }

    // 专门处理用户的读写事件
    void onMessage(const TcpConnectionPtr &conn, // 连接
                Buffer *buffer, // 缓冲区
                Timestamp time) // 接收到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recieve data:" << buf << "time: " 
        << time.toString() << endl;

        conn->send(buf);
    }

    TcpServer _server;
    EventLoop *_loop;
};

int main() {
    EventLoop loop; // epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop(); // epoll_wait阻塞等待新用户连接。
    
    return 0;
}