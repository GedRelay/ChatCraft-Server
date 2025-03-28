#pragma once
#include "const.h"

class HttpConnection: public std::enable_shared_from_this<HttpConnection>{
    // friend class LogicSystem;
public:
    HttpConnection(tcp::socket socket);
    void Start();
private:
    void CheckDeadline();
    void WriteResponse();
    void HandleRequest();  // 处理请求
    void ParseGetParams(std::string &url, std::unordered_map<std::string, std::string> &params);  // 解析GET参数

    tcp::socket _socket;
    beast::flat_buffer _buffer{ 8192 };
    http::request<http::dynamic_body> _request;
    http::response<http::dynamic_body> _response;
    net::steady_timer _deadline{ _socket.get_executor(), std::chrono::seconds(10) };  // 定时器
};