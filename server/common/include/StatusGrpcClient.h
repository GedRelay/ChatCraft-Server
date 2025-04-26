#pragma once
#include "const.h"
#include <grpcpp/grpcpp.h>
#include "proto/message.grpc.pb.h"
#include "StatusStubPool.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::StatusService;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;


class StatusGrpcClient : public Singleton<StatusGrpcClient> {
    friend class Singleton<StatusGrpcClient>;
public:
    GetChatServerRsp GetChatServer(const int& uid);
    LoginRsp Login(const int& uid, const std::string& token);

private:
    StatusGrpcClient();
};
