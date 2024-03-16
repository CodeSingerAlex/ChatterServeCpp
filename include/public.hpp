#ifndef PUBLIC_H
#define PUBLIC_H

/*
service 和 client 的公共文件
*/

enum EnMsgType {
    LOGIN_MSG = 1, // 登录消息
    REG_MSG, // 注册消息
    REG_MSG_ACK, // 注册响应消息
    LOGIN_MSG_ACK, // 登录响应消息
    ONE_CHAT_MSG, // 聊天消息
};

#endif