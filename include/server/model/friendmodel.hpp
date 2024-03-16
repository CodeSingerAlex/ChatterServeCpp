#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "user.hpp"
#include <vector>
using namespace std;

// 维护好友列表
class FriendModel {
public:
    // 添加好友列表
    void insert(int userid, int friendid);
    
    // 返回用户好友列表
    vector<User> query(int userid); 
};
#endif