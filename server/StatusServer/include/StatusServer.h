#include "const.h"
#include <grpcpp/grpcpp.h>
#include "proto/message.grpc.pb.h"
#include "RedisManager.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::StatusService;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;


struct ChatServer{
    std::string name;  // 服务器名称
    std::string host;
    std::string port;
    int connect_count;  // 连接数量
};


// 继承自 gRPC 生成的 StatusService 类
class StatusServer final : public message::StatusService::Service {
public:
    StatusServer();
    Status GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* response) override;
    Status Login(ServerContext* context, const LoginReq* request, LoginRsp* response) override;

private:
    std::unordered_map<std::string, ChatServer> _chat_servers;  // 存储聊天服务器的信息
    std::mutex _getchat_mutex;  // 互斥锁，用于保护 _chat_server_index 的访问
    std::mutex _login_mutex;  // 互斥锁，用于保护登录操作
};