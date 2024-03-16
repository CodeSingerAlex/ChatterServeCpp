#include "offlinemessagemodel.hpp"
#include "db.hpp"

#include<iostream>
#include<vector>
using namespace std;

void OfflineMsgModel::insert(int userid, string msg) {

    char sql[100] = {0};
    sprintf(sql, "insert into offlinemessage value(%d, '%s')", userid, msg.c_str());

    Mysql mysql;
    if(mysql.connect()) {
        mysql.update(sql);
    }
}

void OfflineMsgModel::remove(int userid) {

    char sql[100] = {0};
    sprintf(sql, "delete from offlinemessage where userid = %d", userid);

    Mysql mysql;
    if(mysql.connect()) {
        mysql.update(sql);
    }
}

vector<string> OfflineMsgModel::query(int userid){

    char sql[100] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", userid);

    Mysql mysql;

    vector<string> vec;
    if(mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);

        if(res != nullptr) {
            MYSQL_ROW row;

            while((row = mysql_fetch_row(res)) != nullptr) {
                vec.push_back(row[0]);
            }

            mysql_free_result(res);
        }
    }

    return vec;
}