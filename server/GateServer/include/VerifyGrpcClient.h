#pragma once
#include "const.h"
#include <grpcpp/grpcpp.h>
#include "../../common/proto/message.grpc.pb.h"
#include "../../common/include/Singleton.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class VerifyGrpcClient : public Singleton<VerifyGrpcClient> {
    friend class Singleton<VerifyGrpcClient>;
public:
    GetVarifyRsp GetVarifyCode(const std::string& email);

private:
    VerifyGrpcClient();
    
    std::unique_ptr<VarifyService::Stub> _stub;
};