#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User {
public:
    User() {
        _id = 0;
        _name = "";
        _password = "";
        _state = "";
    }
    
    void setId(int id) {
        _id = id;
    }
    int getId() {
        return _id;
    }

    void setName(string name) {
        _name = name;
    }
    string getName() {
        return _name;
    }

    void setPassword(string password) {
        _password = password;
    }
    string getPassword() {
        return _password;
    }

    void setState(string state) {
        _state = state;
    }
    string getState() {
        return _state;
    }
private:
    int _id;
    string _name;
    string _password;
    string _state;
};

#endif