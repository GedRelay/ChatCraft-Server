#include "../include/LogicSystem.h"
#include "../include/HttpConnection.h"
#include "../include/VerifyGrpcClient.h"
#include "../include/StatusGrpcClient.h"

LogicSystem::LogicSystem(){
    // GET请求处理函数
    _get_handlers["/get_test"] = [](std::unordered_map<std::string, std::string> &params, http::response<http::dynamic_body> &response){
        response.set(http::field::content_type, "text/plain");
        beast::ostream(response.body()) << "get_test\r\n";
        for (auto& param : params){
            beast::ostream(response.body()) << param.first << ": " << param.second << "\r\n";
        }
    };

    // POST请求处理函数
    // 用户注册获取验证码
    _post_handlers["/get_verify_code"] = [](Json::Value &src_json, http::response<http::dynamic_body> &response){
        response.set(http::field::content_type, "application/json");
        Json::Value response_json;
        // 检查参数
        if(!src_json.isMember("email")){
            response_json["status"] = -1;
            response_json["msg"] = "email fields not found";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        // 向gRPC服务器发送请求
        GetVerifyRsp grpc_response = VerifyGrpcClient::GetInstance()->RegisterGetVerifyCode(src_json["email"].asString());  // error, email, code
        if(grpc_response.error() != 0){
            response_json["status"] = -1;
            response_json["msg"] = "get verify code from VerifyServer error";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        response_json["status"] = 0;
        response_json["msg"] = "get verify code success";
        beast::ostream(response.body()) << response_json.toStyledString();
    };

    // 重置密码获取验证码
    _post_handlers["/reset_get_verify_code"] = [](Json::Value &src_json, http::response<http::dynamic_body> &response){
        response.set(http::field::content_type, "application/json");
        Json::Value response_json;
        // 检查参数
        if(!src_json.isMember("user") || !src_json.isMember("email")){
            response_json["status"] = -1;
            response_json["msg"] = "fields not complete, need: user, email";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }

        // 检查mysql中是否存在该邮箱, 1表示存在，0表示不存在，-1表示mysql错误
        int exists_email = MysqlManager::GetInstance()->ExistsEmail(src_json["email"].asString());
        if(exists_email == 0){
            response_json["status"] = -1;
            response_json["msg"] = "email not exists";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        else if(exists_email == -1){
            response_json["status"] = -1;
            response_json["msg"] = "mysql error";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }

        // 检查该邮箱对应的用户名是否匹配
        int check_user_email = MysqlManager::GetInstance()->CheckUserAndEmail(src_json["user"].asString(), src_json["email"].asString());
        if(check_user_email == 0){
            response_json["status"] = -1;
            response_json["msg"] = "user and email not match";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        else if(check_user_email == -1){
            response_json["status"] = -1;
            response_json["msg"] = "mysql error";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }

        // 向VerifyServer发送请求，获取验证码
        GetVerifyRsp grpc_response = VerifyGrpcClient::GetInstance()->ResetGetVerifyCode(src_json["email"].asString());  // error, email, code
        if(grpc_response.error() != 0){
            response_json["status"] = -1;
            response_json["msg"] = "get verify code from VerifyServer error";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        response_json["status"] = 0;
        response_json["msg"] = "get verify code success";
        beast::ostream(response.body()) << response_json.toStyledString();
    };

    // 注册用户
    _post_handlers["/user_register"] = [](Json::Value &src_json, http::response<http::dynamic_body> &response){
        response.set(http::field::content_type, "application/json");
        Json::Value response_json;
        // 检查参数
        if(!src_json.isMember("user") || !src_json.isMember("email") || !src_json.isMember("passwd") || !src_json.isMember("passwd2") || !src_json.isMember("verifycode")){
            response_json["status"] = -1;
            response_json["msg"] = "fields not complete, need: user, email, passwd, passwd2, verifycode";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        // 检测参数合法性
        // 1. email合法
        // 2. passwd合法: 密码要求：1. 至少包含 8 个字符，最多包含 20 个字符 2. 必须包含数字和字母 3. 只允许包含以下特殊字符@#$%^&-+=()
        // 3. passwd2合法: 和passwd相同
        if(src_json["passwd"].asString() != src_json["passwd2"].asString()){
            response_json["status"] = -1;
            response_json["msg"] = "passwd not same";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        // 4. verifycode合法： redis中有存储，且与验证码相同
        std::string key = ConfigManager::GetConfigAs<std::string>("Prefix", "register_verifycode_prefix") + src_json["email"].asString();
        if(RedisManager::GetInstance()->Exists(key)){
            std::string verifycode;
            if(RedisManager::GetInstance()->Get(key, verifycode) == false){
                response_json["status"] = -1;
                response_json["msg"] = "get verify code from redis error";
                beast::ostream(response.body()) << response_json.toStyledString();
                return;
            }
            if(verifycode != src_json["verifycode"].asString()){
                std::cout << "verifycode: " << verifycode << std::endl;
                std::cout << "src_json[verifycode]: " << src_json["verifycode"].asString() << std::endl;
                response_json["status"] = -1;
                response_json["msg"] = "verification code is incorrect or expired";
                beast::ostream(response.body()) << response_json.toStyledString();
                return;
            }
        }
        else{
            response_json["status"] = -1;
            response_json["msg"] = "verification code is incorrect or expired";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        // 5. mysql中不存在相同的用户名
        // 6. mysql中不存在相同的邮箱
        // 将注册信息存入mysql，返回0表示用户名或邮箱已存在，返回-1表示mysql插入失败
        int result = MysqlManager::GetInstance()->RegisterUser(src_json["user"].asString(), src_json["email"].asString(), src_json["passwd"].asString());
        if(result == 0){
            response_json["status"] = -1;
            response_json["msg"] = "user or email already exists";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        else if(result == -1){
            response_json["status"] = -1;
            response_json["msg"] = "mysql insert error";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        // 删除redis中的验证码
        RedisManager::GetInstance()->Del(key);
        response_json["status"] = 0;
        response_json["msg"] = "user register success";
        beast::ostream(response.body()) << response_json.toStyledString();
    };

    // 重置密码
    _post_handlers["/reset_password"] = [](Json::Value &src_json, http::response<http::dynamic_body> &response){
        response.set(http::field::content_type, "application/json");
        Json::Value response_json;
        // 检查参数
        if(!src_json.isMember("user") || !src_json.isMember("email") || !src_json.isMember("passwd") || !src_json.isMember("passwd2") || !src_json.isMember("verifycode")){
            response_json["status"] = -1;
            response_json["msg"] = "fields not complete, need: user, email, passwd, passwd2, verifycode";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        // 检测参数合法性
        // 1. email合法
        // 2. passwd合法: 密码要求：1. 至少包含 8 个字符，最多包含 20 个字符 2. 必须包含数字和字母 3. 只允许包含以下特殊字符@#$%^&-+=()
        // 3. passwd2合法: 和passwd相同
        if(src_json["passwd"].asString() != src_json["passwd2"].asString()){
            response_json["status"] = -1;
            response_json["msg"] = "passwd not same";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        // 4. verifycode合法： redis中有存储，且与验证码相同
        std::string key = ConfigManager::GetConfigAs<std::string>("Prefix", "reset_verifycode_prefix") + src_json["email"].asString();
        if(RedisManager::GetInstance()->Exists(key)){
            std::string verifycode;
            if(RedisManager::GetInstance()->Get(key, verifycode) == false){
                response_json["status"] = -1;
                response_json["msg"] = "get verify code from redis error";
                beast::ostream(response.body()) << response_json.toStyledString();
                return;
            }
            if(verifycode != src_json["verifycode"].asString()){
                std::cout << "verifycode: " << verifycode << std::endl;
                std::cout << "src_json[verifycode]: " << src_json["verifycode"].asString() << std::endl;
                response_json["status"] = -1;
                response_json["msg"] = "verification code is incorrect or expired";
                beast::ostream(response.body()) << response_json.toStyledString();
                return;
            }
        }
        else{
            response_json["status"] = -1;
            response_json["msg"] = "verification code is incorrect or expired";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }

        // 5. 检查mysql中是否存在该邮箱, 1表示存在，0表示不存在，-1表示mysql错误
        int exists_email = MysqlManager::GetInstance()->ExistsEmail(src_json["email"].asString());
        if(exists_email == 0){
            response_json["status"] = -1;
            response_json["msg"] = "email not exists";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        else if(exists_email == -1){
            response_json["status"] = -1;
            response_json["msg"] = "mysql error";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }

        // 6. 检查该邮箱对应的用户名是否匹配
        int check_user_email = MysqlManager::GetInstance()->CheckUserAndEmail(src_json["user"].asString(), src_json["email"].asString());
        if(check_user_email == 0){
            response_json["status"] = -1;
            response_json["msg"] = "user and email not match";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        else if(check_user_email == -1){
            response_json["status"] = -1;
            response_json["msg"] = "mysql error";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }

        // 7. 将新密码存入mysql，返回0表示用户名或邮箱不存在，返回1表示成功，返回2表示新密码和旧密码相同，返回-1表示mysql修改失败
        int result = MysqlManager::GetInstance()->ResetPassword(src_json["user"].asString(), src_json["email"].asString(), src_json["passwd"].asString());
        if(result == 0){
            response_json["status"] = -1;
            response_json["msg"] = "user or email not exists";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        else if(result == 2){
            response_json["status"] = -1;
            response_json["msg"] = "new password same as old password";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        else if(result == -1){
            response_json["status"] = -1;
            response_json["msg"] = "mysql error";
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        // 删除redis中的验证码
        RedisManager::GetInstance()->Del(key);
        response_json["status"] = 0;
        response_json["msg"] = "reset password success";
        beast::ostream(response.body()) << response_json.toStyledString();
    };

    // 用户登录
    _post_handlers["/user_login"] = [](Json::Value &src_json, http::response<http::dynamic_body> &response){
        std::cout << "user login" << std::endl;
        response.set(http::field::content_type, "application/json");
        Json::Value response_json;
        // 检查参数
        if(!src_json.isMember("email") || !src_json.isMember("password")){
            response_json["status"] = -1;
            response_json["msg"] = "fields not complete, need: email, password";
            std::cout << "fields not complete, need: email, password" << std::endl;
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }

        // mysql检查email是否被注册
        int exists_email = MysqlManager::GetInstance()->ExistsEmail(src_json["email"].asString());
        if(exists_email == 0){
            response_json["status"] = -1;
            response_json["msg"] = "account not exists";
            std::cout << "account not exists" << std::endl;
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        else if(exists_email == -1){
            response_json["status"] = -1;
            response_json["msg"] = "mysql error";
            std::cout << "mysql error" << std::endl;
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }

        // mysql检查密码是否正确
        UserInfo userInfo;
        int check_user_passwd = MysqlManager::GetInstance()->CheckEmailAndPassword(src_json["email"].asString(), src_json["password"].asString(), userInfo);
        if(check_user_passwd == 0){
            response_json["status"] = -1;
            response_json["msg"] = "password is incorrect";
            std::cout << "password is incorrect" << std::endl;
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        else if(check_user_passwd == -1){
            response_json["status"] = -1;
            response_json["msg"] = "mysql error";
            std::cout << "mysql error" << std::endl;
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }

        // 查询StatusServer，找到合适的连接
        auto reply = StatusGrpcClient::GetInstance()->GetChatServer(userInfo.uid);
        if(reply.error() != ErrorCodes::SUCCESS){
            response_json["status"] = -1;
            response_json["msg"] = "get chat server from StatusServer error";
            std::cout << "get chat server from StatusServer error" << std::endl;
            beast::ostream(response.body()) << response_json.toStyledString();
            return;
        }
        
        std::cout << "user login success" << std::endl;
        response_json["status"] = 0;
        response_json["msg"] = "user login success";
        response_json["uid"] = std::to_string(userInfo.uid);
        response_json["name"] = userInfo.name;
        response_json["host"] = reply.host();
        response_json["port"] = reply.port();
        response_json["token"] = reply.token();
        std::cout << "uid: " << userInfo.uid << std::endl;
        std::cout << "name: " << userInfo.name << std::endl;
        std::cout << "email: " << userInfo.email << std::endl;
        std::cout << "host: " << reply.host() << std::endl;
        std::cout << "port: " << reply.port() << std::endl;
        std::cout << "token: " << reply.token() << std::endl;
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
    std::cout << "post_url: " << post_url << std::endl;
    std::cout << "json_str: " << json_str << std::endl;
    Json::Value src_json;
    response.result(http::status::ok);
    response.set(http::field::server, "GateServer");
    // 解析json失败
    if(!_json_reader.parse(json_str, src_json)){  
        response.set(http::field::content_type, "application/json");
        std::cerr << "json parse error" << std::endl;
        Json::Value response_json;
        response_json["status"] = -1;
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