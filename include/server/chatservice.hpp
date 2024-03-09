#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>

using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "json.hpp"
using json = nlohmann::json;

using HandlFunc = std::function<void(const TcpConnectionPtr& conn, json& js, Timestamp time)>;


class ChatService {
public:
    // 获取单例对象的接口函数
    static ChatService *instance();

    // 处理登录业务
    void login(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 处理注册业务
    void reg(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 获取消息对应的处理器
    HandlFunc getHandler(int msgid);
private:
    ChatService();

    // 匹配消息id和业务处理方法
    unordered_map<int, HandlFunc> _msgHandlerMap;
};
#endif