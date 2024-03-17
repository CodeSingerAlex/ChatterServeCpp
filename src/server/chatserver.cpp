#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp" 
#include <iostream>
#include <functional>
#include <string>


using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg)
    : server_(loop, listenAddr, nameArg),
        loop_(loop)
        {
            // 设置连接回调
            server_.setConnectionCallback(
                std::bind(&ChatServer::onConnection, this, _1)
            );
            // 设置消息回调
            server_.setMessageCallback(
                std::bind(&ChatServer::onMessage, this, _1, _2, _3)
            );
            // 设置线程数量
            server_.setThreadNum(4); 
        }

// 启动服务
void ChatServer::start() {
    server_.start();
}

// 上报连接信息
void ChatServer::onConnection(const TcpConnectionPtr& conn) {
    if (!conn->connected()) {
        ChatService::instance()->clientCloseException(conn);
        conn -> shutdown();
    }
}

// 上报读写事件信息
void ChatServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time) {
    string buf = buffer->retrieveAllAsString();
    json js = json::parse(buf);
    cout << js.dump() << js["msgid"] << endl;
    auto it = ChatService::instance()->getHandler(js["msgid"].get<int>());
    it(conn, js, time);
}


