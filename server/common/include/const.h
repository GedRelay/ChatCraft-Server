#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "Singleton.h"
#include <functional>
#include <unordered_map>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include "ConfigManager.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstring>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


enum CONST{
    MSG_HEAD_ID_BYTE = 2,  // 消息头中消息id的长度（字节）
    MSG_HEAD_LEN_BYTE = 2,  // 消息头中消息长度的长度（字节）
    MSG_BODY_MAX_LEN_BYTE = 2048,  // 消息体最大长度（字节）
    MSG_PACK_MAX_SIZE_BYTE = 4096,  // 消息包最大长度（字节）确保大于消息头长度+消息体长度
    GATESERVER_IO_CONTEXT_POOL_SIZE = 2,  // GateServer的IO上下文池大小
    CHATSERVER_IO_CONTEXT_POOL_SIZE = 2,  // ChatServer的IO上下文池大小
    VERIFY_STUB_POOL_SIZE = 4,  // 验证服务器的连接池大小
    STATUS_STUB_POOL_SIZE = 4,  // 状态服务器的连接池大小
    REDIS_CONNECTION_POOL_SIZE = 4,  // Redis连接池大小
    MYSQL_CONNECTION_POOL_SIZE = 4  // MySQL连接池大小
};


enum ErrorCodes{
    SUCCESS = 0,  // 成功
    UidInvalid = 1,  // 用户ID无效
    TokenInvalid = 2,  // token无效
    RedisError = 3,  // Redis错误
    MysqlError = 4,  // MySQL错误
};