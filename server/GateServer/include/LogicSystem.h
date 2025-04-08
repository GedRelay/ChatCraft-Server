#pragma once
#include "const.h"
#include "RedisManager.h"
#include "MysqlManager.h"

class HttpConnection;

typedef std::function<void(std::unordered_map<std::string, std::string>&, http::response<http::dynamic_body>&)> HttpGetHandler;
typedef std::function<void(Json::Value&, http::response<http::dynamic_body>&)> HttpPostHandler;

class LogicSystem: public Singleton<LogicSystem>{
    friend class Singleton<LogicSystem>;
public:
    bool HandleGet(const http::request<http::dynamic_body> &request, http::response<http::dynamic_body>& response);
    bool HandlePost(const http::request<http::dynamic_body> &request, http::response<http::dynamic_body>& response);
private:
    LogicSystem();
    void ParseGetParams(const boost::beast::string_view &trarget, std::string &get_url, std::unordered_map<std::string, std::string> &get_params);  // 解析GET参数
    unsigned char ToHex(unsigned char x);  // 16进制转换，将0-15转换为0-9，A-F
    unsigned char FromHex(unsigned char x);  // 16进制转换，将0-9，A-F转换为0-15
    std::string UrlDecode(const std::string& str);  // URL解码
    std::string UrlEncode(const std::string& str);  // URL编码

    std::unordered_map<std::string, HttpGetHandler> _get_handlers;
    std::unordered_map<std::string, HttpPostHandler> _post_handlers;
    Json::Reader _json_reader;
};