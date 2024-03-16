#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>
#include <string>

using namespace std;

// 数据库操作类
class Mysql {
public:
    // 初始化数据库连接
    Mysql();

    // 释放数据库连接资源
    ~Mysql();

    // 连接数据库
    bool connect();

    // 更新操作
    bool update(string sql);

    // 查询操作
    MYSQL_RES* query(string sql);

    // 获取连接
    MYSQL* getConnection() {
        return _conn;
    }
private:
    MYSQL* _conn;
};
#endif