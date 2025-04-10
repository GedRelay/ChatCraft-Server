#pragma once
#include "const.h"
#include <grpcpp/grpcpp.h>
#include "proto/message.grpc.pb.h"
#include "GrpcStubPool.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

class VerifyGrpcClient : public Singleton<VerifyGrpcClient> {
    friend class Singleton<VerifyGrpcClient>;
public:
    GetVerifyRsp RegisterGetVerifyCode(const std::string& email);
    GetVerifyRsp ResetGetVerifyCode(const std::string& email);

private:
    VerifyGrpcClient();
};