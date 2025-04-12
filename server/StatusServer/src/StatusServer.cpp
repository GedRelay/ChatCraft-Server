#include "../include/StatusServer.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

StatusServer::StatusServer():
    _chat_server_index(0) {
    ChatServer server1;
    server1.port = ConfigManager::GetConfigAs("ChatServer1", "port");
    server1.host = ConfigManager::GetConfigAs("ChatServer1", "host");
    _chat_servers.push_back(server1);

    ChatServer server2;
    server2.port = ConfigManager::GetConfigAs("ChatServer2", "port");
    server2.host = ConfigManager::GetConfigAs("ChatServer2", "host");
    _chat_servers.push_back(server2);
}


Status StatusServer::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* response) {
    auto& server = _chat_servers[_chat_server_index];
    _chat_server_index = (_chat_server_index + 1) % _chat_servers.size();

    // 生成随机token
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::string token = boost::uuids::to_string(uuid);

    response->set_host(server.host);
    response->set_port(server.port);
    response->set_token(token);
    response->set_error(ErrorCodes::SUCCESS);
    
    std::cout << "user_id: " << request->uid() << std::endl;
    std::cout << "GetChatServer: " << server.host << ":" << server.port << ", token: " << token << std::endl;

    return Status::OK;
}