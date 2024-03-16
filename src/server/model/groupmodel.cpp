#include "groupmodel.hpp"
#include "db.hpp"

bool GroupModel::createGroup(Group &group) {
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')",
            group.getName().c_str(), group.getDesc().c_str());

    Mysql mysql;

    if (mysql.connect()) {
        if (mysql.update(sql)) {
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }

    return false; 
}

void GroupModel::addGroup(int userid, int groupid, string role) {
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d, %d, '%s')", userid, groupid, role.c_str());

    Mysql mysql;

    if(mysql.connect()) {
        mysql.update(sql);
    }
}

vector<Group> GroupModel::queryGroups(int userid) {
    char sql[1024] = {0};
    sprintf(sql, "select ag.id, ag.groupname, ag.groupdesc from allgroup as ag inner join groupuser as gu on ag.id = gu.groupid where gu.userid = %d", userid);

    vector<Group> groupVec;

    Mysql mysql;
    if(mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr) {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr) {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    for(Group &group: groupVec) {
        sprintf(sql, "select u.id, u.name, u.state, g.role from user as u inner join groupuser as g on u.id = g.userid where g.groupid = %d", group.getId());
        
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr) {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr) {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}

vector<int> GroupModel::queryGroupUsers(int userid, int groupid) {
    char sql[1024];
    sprintf(sql, "select userid from groupuser where groupid = %d and where userid = %d", groupid, userid);

    vector<int> vec;

    Mysql mysql;
    if(mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr) {
            MYSQL_ROW row;  
            while((row = mysql_fetch_row(res)) != nullptr) {
                vec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }

    return vec;
}