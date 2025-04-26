#include "../include/StatusServer.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

StatusServer::StatusServer(){
    ChatServer server1;
    server1.name = "ChatServer1";
    server1.port = ConfigManager::GetConfigAs("ChatServer1", "port");
    server1.host = ConfigManager::GetConfigAs("ChatServer1", "host");
    server1.connect_count = 0;  // 初始化连接数为0
    _chat_servers["ChatServer1"] = server1;

    ChatServer server2;
    server2.name = "ChatServer2";
    server2.port = ConfigManager::GetConfigAs("ChatServer2", "port");
    server2.host = ConfigManager::GetConfigAs("ChatServer2", "host");
    server2.connect_count = 0;  // 初始化连接数为0
    _chat_servers["ChatServer2"] = server2;
}


Status StatusServer::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* response) {
    std::lock_guard<std::mutex> lock(_getchat_mutex);  // 互斥锁

    std::cout << "收到分配ChatServer请求, uid:" << request->uid() << std::endl;

    // 找到连接数最少的聊天服务器
    ChatServer chat_server = _chat_servers.begin()->second;
    for (const auto& server : _chat_servers) {
        if (server.second.connect_count < chat_server.connect_count) {
            chat_server = server.second;
        }
    }

    // 生成随机token
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::string token = boost::uuids::to_string(uuid);

    response->set_host(chat_server.host);
    response->set_port(chat_server.port);
    response->set_token(token);
    response->set_error(ErrorCodes::SUCCESS);

    // 将token记入redis
    std::string token_key = ConfigManager::GetConfigAs("Prefix", "user_token_prefix") + std::to_string(request->uid());
    bool success = RedisManager::GetInstance()->Set(token_key, token);
    if(!success) {
        std::cout << "Redis设置token失败" << std::endl;
        response->set_error(ErrorCodes::RedisError);
        return Status::OK;
    }

    std::cout << "分配的ChatServer: " << chat_server.name << std::endl;
    std::cout << "token: " << token << std::endl;

    return Status::OK;
}



Status StatusServer::Login(ServerContext* context, const LoginReq* request, LoginRsp* response){
    std::lock_guard<std::mutex> lock(_login_mutex);  // 互斥锁

    std::cout << "收到登录请求, uid:" << request->uid() << ", token:" << request->token() << std::endl;
    // redis中检查uid和token是否匹配

    std::string token_key = ConfigManager::GetConfigAs("Prefix", "user_token_prefix") + std::to_string(request->uid());
    std::string token = "";
    bool success = RedisManager::GetInstance()->Get(token_key, token);

    if(!success) {
        std::cout << "token不存在，登录失败" << std::endl;
        response->set_error(ErrorCodes::UidInvalid);
        return Status::OK;
    }

    if(token != request->token()) {
        std::cout << "token不匹配，登录失败" << std::endl;
        response->set_error(ErrorCodes::TokenInvalid);
        return Status::OK;
    }

    // 登录成功
    response->set_error(ErrorCodes::SUCCESS);
    response->set_uid(request->uid());
    response->set_token(request->token());
    std::cout << "用户登录成功, uid: " << request->uid() << ", token: " << request->token() << std::endl;
    return Status::OK;
}