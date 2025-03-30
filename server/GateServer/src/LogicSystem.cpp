#include "../include/LogicSystem.h"
#include "../include/HttpConnection.h"
#include "../include/VerifyGrpcClient.h"

LogicSystem::LogicSystem(){
    // GET请求处理函数
    _get_handlers["/get_test"] = [](std::unordered_map<std::string, std::string> &params, http::response<http::dynamic_body> &response){
        beast::ostream(response.body()) << "get_test\r\n";
        for (auto& param : params){
            beast::ostream(response.body()) << param.first << ": " << param.second << "\r\n";
        }
    };

    // POST请求处理函数
    _post_handlers["/get_verify_code"] = [](Json::Value &src_json, http::response<http::dynamic_body> &response){
        Json::Value response_json;
        if(!src_json.isMember("email")){
            response_json["status"] = "error";
            response_json["msg"] = "email not found";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        // 向gRPC服务器发送请求
        GetVarifyRsp grpc_response = VerifyGrpcClient::GetInstance()->GetVarifyCode(src_json["email"].asString());  // error, email, code
        if(grpc_response.error() != 0){
            response_json["status"] = "error";
            response_json["msg"] = "get verify code from VerifyServer error";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        response_json["status"] = "ok";
        response_json["msg"] = "get verify code success";
        response_json["verify_code"] = grpc_response.code();
        beast::ostream(response.body()) << response_json.toStyledString();
    };
}


// 外部调用HandleGet，先解析GET链接和参数，然后调用对应的处理函数
bool LogicSystem::HandleGet(const http::request<http::dynamic_body> &request, http::response<http::dynamic_body>& response){
    // 解析GET链接和参数
    std::string get_url;
    std::unordered_map<std::string, std::string> params;
    ParseGetParams(request.target(), get_url, params);
    // url没有对应的处理函数
    if(_get_handlers.find(get_url) == _get_handlers.end()){
        response.result(http::status::not_found);
        response.set(http::field::content_type, "text/plain");
        beast::ostream(response.body()) << "url not found\r\n";
        return false;
    }
    // 调用对应的处理函数
    _get_handlers[get_url](params, response);
    response.result(http::status::ok);
    response.set(http::field::server, "GateServer");
    response.set(http::field::content_type, "application/json");
    return true;
}


// 外部调用HandlePost，先将POST内容转换为json，然后调用对应的处理函数
bool LogicSystem::HandlePost(const http::request<http::dynamic_body> &request, http::response<http::dynamic_body>& response){
    // 得到链接
    auto post_url = request.target();
    // url没有对应的处理函数
    if(_post_handlers.find(post_url) == _post_handlers.end()){
        response.result(http::status::not_found);
        response.set(http::field::content_type, "text/plain");
        beast::ostream(response.body()) << "url not found\r\n";
        return false;
    }
    // 解析json
    auto json_str = boost::beast::buffers_to_string(request.body().data());
    std::cout << "json_str: " << json_str << std::endl;
    Json::Value src_json;
    response.result(http::status::ok);
    response.set(http::field::server, "GateServer");
    response.set(http::field::content_type, "application/json");
    // 解析json失败
    if(!_json_reader.parse(json_str, src_json)){  
        std::cerr << "json parse error" << std::endl;
        Json::Value response_json;
        response_json["status"] = "error";
        response_json["msg"] = "json parse error";
        beast::ostream(response.body()) << response_json.toStyledString();
        return false;
    }
    // 调用对应的处理函数
    _post_handlers[post_url](src_json, response);
    return true;
}




// 解析GET参数, 将url存入get_url, 将参数存入get_params
void LogicSystem::ParseGetParams(const boost::beast::string_view &url, std::string &get_url, std::unordered_map<std::string, std::string> &get_params){
    size_t pos = url.find("?");
    if(pos == std::string::npos){  // 没有参数
        get_url = url;
        return;
    }
    get_url = url.substr(0, pos);
    std::string params_str = url.substr(pos + 1);
    std::string key, value;

    while((pos = params_str.find("&")) != std::string::npos){
        size_t equal_pos = params_str.find("="); 
        if(equal_pos != std::string::npos){
            key = UrlDecode(params_str.substr(0, equal_pos));
            value = UrlDecode(params_str.substr(equal_pos + 1, pos - equal_pos - 1));
            get_params[key] = value;
        }
        params_str = params_str.substr(pos + 1);      
    }
    if(params_str.size() > 0){  // 最后一个参数没有&，单独处理
        size_t equal_pos = params_str.find("=");
        if(equal_pos != std::string::npos){
            key = UrlDecode(params_str.substr(0, equal_pos));
            value = UrlDecode(params_str.substr(equal_pos + 1));
            get_params[key] = value;
        }
    }
}




// 16进制转换，将0-15转换为0-9，A-F
unsigned char LogicSystem::ToHex(unsigned char x) { 
    return  x > 9 ? x + 55 : x + 48; 
}

// 16进制转换，将0-9，A-F转换为0-15
unsigned char LogicSystem::FromHex(unsigned char x) { 
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else y = 0;
    return y;
}


// url解码
std::string LogicSystem::UrlDecode(const std::string& str) {
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '+') strTemp += ' ';  // +号转换为空格
        else if (str[i] == '%') {  // %号后面跟着两个16进制数
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}


// url编码
std::string LogicSystem::UrlEncode(const std::string& str) {
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (isalnum((unsigned char)str[i]) || 
            (str[i] == '-') ||
            (str[i] == '_') || 
            (str[i] == '.') || 
            (str[i] == '~')) {  // 数字，字母，-，_，.，~不转换
            strTemp += str[i];
        }
        else if (str[i] == ' ') {  // 空格转换为+
            strTemp += '+';
        }
        else {  // 其他字符转换为%XX的形式（XX是字符的16进制ASCII码）
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] & 0x0f);
        }
    }
    return strTemp;
}