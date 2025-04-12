#include "const.h"
#include <grpcpp/grpcpp.h>
#include "proto/message.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;


struct ChatServer{
    std::string host;
    std::string port;
};


// 继承自 gRPC 生成的 StatusService 类
class StatusServer final : public message::StatusService::Service {
public:
    StatusServer();
    Status GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* response) override;

private:
    std::vector<ChatServer> _chat_servers;  // 存储聊天服务器的信息
    size_t _chat_server_index;  // 当前使用的聊天服务器索引
};