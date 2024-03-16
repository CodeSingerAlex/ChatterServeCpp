#include <mysql/mysql.h>
#include <iostream>
#include <string>
using namespace std;

int main() {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    string server = "43.136.51.98";
    string user = "root";
    string password = "123456";
    string dbname = "chat";

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 0, NULL, 0)) {
        std::cerr << "Connection error: " << mysql_error(conn) << std::endl;
        return 1;
    }

    std::cout << "Connected to database successfully!" << std::endl;

    mysql_close(conn);
    return 0;
}