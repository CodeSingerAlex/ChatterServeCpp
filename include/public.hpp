#ifndef PUBLIC_H
#define PUBLIC_H

#include <string>
using namespace std;
/*
service 和 client 的公共文件
*/
// 登录状态
const static string ONLINE = "online";
const static string OFFLINE = "offline";

// 群内身份
const static string CREATOR = "creator";
const static string NORMAL = "normal";

enum EnMsgType {
    LOGIN_MSG = 1, // 登录消息 1
    LOGINOUT_MSG, // 注销消息 2
    REG_MSG, // 注册消息 3

    REG_MSG_ACK, // 注册响应消息 4
    LOGIN_MSG_ACK, // 登录响应消息 5

    ONE_CHAT_MSG, // 聊天消息 6
    
    ADD_FRIEND_MSG, // 添加好友 7

    GREATE_GROUP_MSG, // 创建群组 8
    ADD_GROUP_MSG, // 添加群组 9
    GROUP_CHAT_MSG // 群聊天 10
};

#endif