#pragma once
#include "const.h"
#include <grpcpp/grpcpp.h>
#include "proto/message.grpc.pb.h"

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;


class GrpcStubPool : public Singleton<GrpcStubPool> {
    friend class Singleton<GrpcStubPool>;
public:
    ~GrpcStubPool();
    std::unique_ptr<VarifyService::Stub> GetVerifyStub();  // 获取一个 gRPC Stub
    void ReturnVerifyStub(std::unique_ptr<VarifyService::Stub> stub);  // 归还 gRPC Stub
    void Close();  // 关闭 gRPC Stub 池
private:
    GrpcStubPool();

    bool _is_shutdown;
    size_t _pool_size;
    std::queue<std::unique_ptr<VarifyService::Stub>> _stub_queue;
    std::mutex _mutex;
    std::condition_variable _cond_var;
};