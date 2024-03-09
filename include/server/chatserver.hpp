#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <string>

using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    // 初始化服务器
    ChatServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg);

    // 启动服务器
    void start();
private:
    // 上报连接信息
    void onConnection(const TcpConnectionPtr&);

    // 上报读写事件信息
    void onMessage(const TcpConnectionPtr&, Buffer*, Timestamp);

private:
    // 服务器对象
    TcpServer server_;

    // 事件循环对象
    EventLoop* loop_;
};
#endif