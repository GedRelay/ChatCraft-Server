#include "../include/RedisManager.h"


RedisManager::RedisManager() :
    _is_shutdown(false),
    _pool_size(0) {
    std::string REDIS_HOST = ConfigManager::GetConfigAs("Redis", "host");
    int REDIS_PORT = ConfigManager::GetConfigAs<int>("Redis", "port");
    std::string REDIS_PASSWORD = ConfigManager::GetConfigAs("Redis", "pass");

    for (int i = 0; i < CONST::REDIS_CONNECTION_POOL_SIZE; ++i) {
        // 连接Redis
        redisContext* context = nullptr;
        if (Connnect(REDIS_HOST, REDIS_PORT, context) == false){
            continue;
        }
        // 认证
        if (Auth(context, REDIS_PASSWORD) == false) {
            redisFree(context);
            continue;
        }
        // 将连接放入连接池
        _redis_connections_pool.push(context);
        ++_pool_size;
    }
    if( _redis_connections_pool.size() == 0) {
        throw std::runtime_error("创建Redis连接池失败");
    }
}


RedisManager::~RedisManager() {
    Close();
}


void RedisManager::Close() {
    _is_shutdown = true;
    _cond.notify_all();
    // 等待所有Redis连接被归还，即连接池大小为_pool_size
    std::unique_lock<std::mutex> lock(_mutex);
    while (_redis_connections_pool.size() < _pool_size) {
        _cond.wait(lock);
    }
    // 释放所有Redis连接
    while (!_redis_connections_pool.empty()) {
        redisContext* context = _redis_connections_pool.front();
        _redis_connections_pool.pop();
        redisFree(context);
    }
    _pool_size = 0;
}


// 获取Redis连接
redisContext* RedisManager::GetRedisConnection() {
    std::unique_lock<std::mutex> lock(_mutex);
    while (_redis_connections_pool.empty() && !_is_shutdown) {
        _cond.wait(lock);
    }
    if (_is_shutdown) {
        return nullptr;
    }
    redisContext* context = _redis_connections_pool.front();
    _redis_connections_pool.pop();
    return context;
}


// 归还Redis连接
void RedisManager::returnRedisConnection(redisContext* context) {
    std::unique_lock<std::mutex> lock(_mutex);
    _redis_connections_pool.push(context);
    _cond.notify_one();
}


// 连接Redis，成功返回true，失败返回false
bool RedisManager::Connnect(const std::string& host, int port, redisContext* &context) {
    context = redisConnect(host.c_str(), port);
    if(context == nullptr) {
        return false;
    }
    if (context->err) {
        redisFree(context);
        return false;
    }
    return true;
}


// 认证
bool RedisManager::Auth(redisContext* &context, const std::string& password) {
    redisReply* reply = (redisReply*)redisCommand(context, "AUTH %s", password.c_str());
    if (reply == nullptr) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        freeReplyObject(reply);
        return false;
    }
    freeReplyObject(reply);
    return true;
}


// 获取key的值
bool RedisManager::Get(const std::string& key, std::string& value) {
    redisContext* context = GetRedisConnection();
    if (context == nullptr) {  // 连接池已关闭
        return false;
    }
    redisReply* reply = (redisReply*)redisCommand(context, "GET %s", key.c_str());
    if (reply == nullptr) {  // 执行命令失败
        returnRedisConnection(context);
        return false;
    }
    if( reply->type == REDIS_REPLY_ERROR) {  // 获取失败
        freeReplyObject(reply);
        returnRedisConnection(context);
        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
    returnRedisConnection(context);
    return true;
}


// 设置key的值
bool RedisManager::Set(const std::string& key, const std::string& value) {
    redisContext* context = GetRedisConnection();
    if (context == nullptr) {  // 连接池已关闭
        return false;
    }
    redisReply* reply = (redisReply*)redisCommand(context, "SET %s %s", key.c_str(), value.c_str());
    if (reply == nullptr) {  // 执行命令失败
        returnRedisConnection(context);
        return false;
    }
    if (!(reply->type == REDIS_REPLY_STATUS && (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0))) {  // 设置失败
        freeReplyObject(reply);
        returnRedisConnection(context);
        return false;
    }
    freeReplyObject(reply);
    returnRedisConnection(context);
    return true;
}


// 判断key是否存在
bool RedisManager::Exists(const std::string& key) {
    redisContext* context = GetRedisConnection();
    if (context == nullptr) {  // 连接池已关闭
        return false;
    }
    redisReply* reply = (redisReply*)redisCommand(context, "EXISTS %s", key.c_str());
    if (reply == nullptr) {  // 执行命令失败
        returnRedisConnection(context);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER) {  // 判断失败
        freeReplyObject(reply);
        returnRedisConnection(context);
        return false;
    }
    bool exists = reply->integer == 1;
    freeReplyObject(reply);
    returnRedisConnection(context);
    return exists;
}