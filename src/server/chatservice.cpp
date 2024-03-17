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
    // 用户基本业务管理相关事件处理回调注册
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&ChatService::loginout, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});

    // 群组业务相关事件处理回调注册
    _msgHandlerMap.insert({GREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
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
                response["friends"] = friendsVec;
            }

            // 群组信息查询
            vector<Group> groupVec = _groupModel.queryGroups(id);
            if(!groupVec.empty()) {
                vector<string> groupV;
                for(Group &group: groupVec) {
                    json js;
                    js["id"] = group.getId();
                    js["groupname"] = group.getName();
                    js["groupdesc"] = group.getDesc();

                    vector<string> userV;
                    for(GroupUser &user : group.getUsers()) {
                        json ujs;
                        ujs["id"] = user.getId();
                        ujs["name"] = user.getName();
                        ujs["state"] = user.getState();
                        ujs["role"] = user.getRole();
                        userV.push_back(ujs.dump());
                    }
                    js["users"] = userV;
                    groupV.push_back(js.dump());
                }
                response["groups"] = groupV;
            }
            string responseStr = response.dump();
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

// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 更新用户的状态信息
    User user(userid, "", "", OFFLINE);
    _userModel.updateState(user);
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

// 添加朋友业务
void ChatService::addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 添加好友信息
    _friendModel.insert(friendid, userid); 
}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    string buffer = js.dump();
    int userid = js["id"];
    string name = js["groupname"];
    string desc = js["groupdesc"];

    Group group(-1, name, desc);
    if(_groupModel.createGroup(group)) {
        _groupModel.addGroup(group.getId(), userid, CREATOR);
    }
}

// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userid = js["id"];
    int groupid = js["groupid"];
    _groupModel.addGroup(groupid, userid, NORMAL);
}

// 群聊天业务
void ChatService::groupChat(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userid = js["id"];
    int groupid = js["groupid"];
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);
    lock_guard<mutex> lock(_connMutex);
    for(int id: useridVec) {
        auto it = _userConnMap.find(id);
        if(it != _userConnMap.end()) {
            it->second->send(js.dump());
        } else {
            _offlineMsgModel.insert(id, js.dump()); 
        }
    }
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

