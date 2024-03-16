#include "db.hpp"

#include <muduo/base/Logging.h>
 
// 配置信息（临时）
static string server = "43.136.51.98";
static string user = "root";
static string password = "123456";
static string dbname = "chat";

Mysql::Mysql() {
    _conn = mysql_init(nullptr);
}

Mysql::~Mysql() {
    if (_conn != nullptr) {
        mysql_close(_conn);
    }
}

bool Mysql::connect() {
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 0, nullptr, 0);
    if (p == nullptr) {
        LOG_ERROR << "connect mysql error:" << mysql_error(_conn); 
    } else {
        mysql_query(_conn, "set names gbk");
    }
    return p;
}

bool Mysql::update(string sql) {
    if(mysql_query(_conn, sql.c_str())) {
        LOG_INFO  << __FILE__ << __LINE__ << ": " << sql  << "更新失败!" << mysql_error(_conn); 
        return false;
    }
    return true;
}

MYSQL_RES* Mysql::query(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << __LINE__ << ": " << sql << "查询失败!" << mysql_error(_conn);
        return nullptr;
    }
    return mysql_use_result(_conn);
}
