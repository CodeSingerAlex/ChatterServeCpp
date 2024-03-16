#include "chatservice.hpp"
#include "public.hpp"
#include "user.hpp"
#include <muduo/base/Logging.h>

using namespace std;
using namespace muduo;

// 获取单例对象的接口函数
ChatService *ChatService::instance() {
    static ChatService service;
    return &service;
}


// 注册消息以及对应的Handler
ChatService::ChatService() {
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
}


// 获取消息对应的处理器
HandlFunc ChatService::getHandler(int msgid) {
    auto it = _msgHandlerMap.find(msgid);
    if(it == _msgHandlerMap.end()) {
        return [=](const TcpConnectionPtr& conn, json& js, Timestamp time) {
            LOG_ERROR << "msgid:" << msgid << "can not find handler!";
        };
    }else {
        return _msgHandlerMap[msgid];
    }
}
// 处理登录业务
void ChatService::login(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.query(id);
    if(user.getId() == id  && user.getPassword() == pwd) {
        // 已经登录
        if(user.getState() == ONLINE) {
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "User already login!";
            conn->send(response.dump());
        } else {
            // 登录成功
            // 更新状态信息
            user.setState(ONLINE);
            _userModel.updateState(user);

            // 记录用户连接信息
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            // {"msgid":4, "errno":0, "errmsg":"login success!"}
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            response["errmsg"] = "Login success!"; 

            vector<string> vec = _offlineMsgModel.query(id);
            if(!vec.empty()) {
                response["offlinemsg"] = vec;
                _offlineMsgModel.remove(id);
            }

            // 好友查询
            vector<User> userVec = _friendModel.query(id); 
            if(!userVec.empty()) {
                vector<string> friendsVec;
                for(User &user: userVec) {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState(); 
                    friendsVec.push_back(js.dump());
                }
            }
            conn->send(response.dump());
        }
    } else {
        // 登录失败
        // {"msgid":4, "errno":1, "errmsg":"login failed!"}
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "Login failed! Name or password is wrong!";
        conn->send(response.dump());
    
    }
}

// 处理注册业务
void ChatService::reg(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    string name = js["name"].get<string>();
    string pwd = js["password"].get<string>();

    User user;
    user.setName(name);
    user.setPassword(pwd);

    bool state = _userModel.insert(user);
    if(state) {
        // 注册成功
        // {"msgid":3, "errno":0, "errmsg":"register success!"}
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        response["errmsg"] = "Register success!";
        conn->send(response.dump());
    }else {
        // 注册失败
        // {"msgid":3, "errno":1, "errmsg":"register failed!"}
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "Register failed!";
        conn->send(response.dump());
    }
}

// 一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int toid = js["toid"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if(it != _userConnMap.end()) {
            // toid在线，转发消息
            it->second->send(js.dump());
            return;
        }

        // 离线
        _offlineMsgModel.insert(toid, js.dump());
    }
}

void ChatService::addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 添加好友信息
    _friendModel.insert(userid, friendid); 
}

// 客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr& conn) {
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for(auto it = _userConnMap.begin(); it != _userConnMap.end(); it++) {
            if(it->second == conn) {
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    // 更新用户的状态信息
    if(user.getId() != 0) {
        user.setState(OFFLINE);
        _userModel.updateState(user);
    }
}

// 重置所有用户的登录状态
void ChatService::reset() {
    _userModel.resetState();
}

