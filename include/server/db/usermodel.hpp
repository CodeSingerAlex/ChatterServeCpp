#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

class UserModel {
public:
    // 插入数据
    bool insert(User &user);

    // 根据用户名查询用户
    User query(int id);

    // 更新用户的状态信息
    bool updateState(const User user);
};
#endif