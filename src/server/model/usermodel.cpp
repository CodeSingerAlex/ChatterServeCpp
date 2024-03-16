#include "usermodel.hpp"
#include "db.hpp"
#include "public.hpp"

#include <iostream>
using namespace std;

bool UserModel::insert(User &user) {
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password) values('%s', '%s')",
            user.getName().c_str(), user.getPassword().c_str());
    // 2. 执行sql语句
    Mysql mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {
            // 获取插入成功的用户数据生成的主键id
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false; 
}

User UserModel::query(int id) {
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);
    Mysql mysql;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr) {
                User user;
                user.setId(atol(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }

    return User();
}

bool UserModel::updateState(User user) {
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d",
            user.getState().c_str(), user.getId());
    // 2. 执行sql语句
    Mysql mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {
            return true;
        }
    }
    return false;
}

void UserModel::resetState() {
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where state = '%s'",
        OFFLINE.c_str(), ONLINE.c_str());
    // 2. 执行sql语句
    Mysql mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}