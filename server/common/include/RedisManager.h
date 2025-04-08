#pragma once
#include "const.h"

class RedisManager : public Singleton<RedisManager> {
friend class Singleton<RedisManager>;
public:
    ~RedisManager();
    
    void Close(); // 关闭Redis连接池
    bool Get(const std::string& key, std::string& value); // 获取key的值
    bool Set(const std::string& key, const std::string& value); // 设置key的值
    bool Exists(const std::string& key); // 判断key是否存在
private:
    RedisManager();
    bool Connnect(const std::string& host, int port, redisContext*& context); // 连接Redis
    bool Auth(redisContext* &context, const std::string& password);  // 认证
    redisContext* GetRedisConnection(); // 获取Redis连接
    void returnRedisConnection(redisContext* context); // 归还Redis连接

    std::atomic<bool> _is_shutdown;
    size_t _pool_size;  // Redis连接池大小
    std::queue<redisContext*> _redis_connections_pool;  // Redis连接池
    std::mutex _mutex;  // 互斥锁
    std::condition_variable _cond;  // 条件变量
};