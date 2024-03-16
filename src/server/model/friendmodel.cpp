#include "friendmodel.hpp"
#include "db.hpp"
#include <vector>
using namespace std;

void FriendModel::insert(int userid, int friendid) {
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)", userid, friendid);
    
    Mysql mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}

vector<User> FriendModel::query(int userid) {
    char sql[1024] = {0};
    sprintf(sql, "select user.id, user.name, user.state from user inner join friend on user.id = friend.friendid where friend.userid = %d", userid);

    Mysql mysql;
    vector<User> vec;

    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);

        while(res != nullptr) {
            MYSQL_ROW row;

            while((row = mysql_fetch_row(res)) != nullptr) {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }

    return vec;
}