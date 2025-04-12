#pragma once
#include "const.h"
#include <grpcpp/grpcpp.h>
#include "proto/message.grpc.pb.h"
#include "StatusStubPool.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

class StatusGrpcClient : public Singleton<StatusGrpcClient> {
    friend class Singleton<StatusGrpcClient>;
public:
    GetChatServerRsp GetChatServer(const int& uid);

private:
    StatusGrpcClient();
};
