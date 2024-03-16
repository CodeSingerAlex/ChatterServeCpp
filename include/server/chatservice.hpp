#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"

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

    // 一对一聊天业务
    void oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time);
    
    // 获取消息对应的处理器
    HandlFunc getHandler(int msgid);

    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr& conn);
    
private:
    ChatService();

    // 匹配消息id和业务处理方法
    unordered_map<int, HandlFunc> _msgHandlerMap;

    // User表的操作对象
    UserModel _userModel;

    // 互斥锁，保证_userModel线程安全
    mutex _connMutex;

    // 存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    // 离线消息存储对象
    OfflineMsgModel _offlineMsgModel;

};
#endif