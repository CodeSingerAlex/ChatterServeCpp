#include <thread>
#include <iostream>
#include "redis.hpp"
using namespace std;
Redis::Redis() {
    _publish_context = nullptr;
    _subscribe_context = nullptr;
}


Redis::~Redis() {
    if (_publish_context != nullptr) {
        redisFree(_publish_context);
    }
    if (_subscribe_context != nullptr) {
        redisFree(_subscribe_context);
    }
}

bool Redis::connect() {
    _publish_context = redisConnect(IP.c_str(), PORT);
    if(_publish_context == nullptr) {
        cerr << "connect redis failed!" << endl;
        return false;
    }

    _subscribe_context = redisConnect(IP.c_str(), PORT);
    if(_subscribe_context == nullptr) {
        cerr << "connect redis failed!" << endl;
        return false;
    }

    // 在单独的线程中监听通道消息，收到消息后，给service层上报
    thread t([&]() {
        observer_channel_message();
    });
    t.detach();

    cout << "connect redis-server success!" << endl;
    return true;
} 

bool Redis::publish(int channel, string message) {
    redisReply* reply = (redisReply*)redisCommand(_publish_context, "PUBLISH %d %s", channel, message.c_str());
    
    if(reply == nullptr) {
        cerr << "publish command failed!" << endl;
        return false;
    }

    freeReplyObject(reply);
    return true;
}

bool Redis::subscribe(int channel) {
    // SUBSCRIBE本身是一个阻塞命令，因此需要在独立的线程中执行
    // 这里只负责发送订阅命令，不负责接收消息
    if(redisAppendCommand(_subscribe_context, "SUBSCRIBE %d", channel) == REDIS_ERR) {
        cerr << "subscribe command failed!" << endl;
        return false;
    }

    int done = 0;
    while(!done) {
        if(redisBufferWrite(_subscribe_context, &done) == REDIS_ERR) {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }

    return true;

}

bool Redis::unsubscribe(int channel) {
    if(redisAppendCommand(_subscribe_context, "UNSUBSCRIBE %d", channel) == REDIS_ERR) {
        cerr << "unsubscribe command failed!" << endl;
        return false;
    }

    int done = 0;
    while(!done) {
        if(redisBufferWrite(_subscribe_context, &done) == REDIS_ERR) {
            cerr << "unsubscribe command failed!" << endl;
            return false;
        }
    } 

    return true;
}

void Redis::observer_channel_message() {
    redisReply* reply = nullptr;
    while(redisGetReply(_subscribe_context, (void**)&reply) == REDIS_OK) {
        if(reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr) {
            _notify_message_handler(atoi(reply->element[1]->str), reply->element[2]->str);
        }
        freeReplyObject(reply);
    }

    cerr << "exit observe thread..." << endl; 
}

void Redis::init_notify_handler(function<void(int, string)> fn) {
    _notify_message_handler = fn;
}