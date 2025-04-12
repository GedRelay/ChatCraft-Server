#pragma once
#include "const.h"
#include <grpcpp/grpcpp.h>
#include "proto/message.grpc.pb.h"

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;


class VerifyStubPool : public Singleton<VerifyStubPool> {
    friend class Singleton<VerifyStubPool>;
public:
    ~VerifyStubPool();
    std::unique_ptr<VerifyService::Stub> GetVerifyStub();  // 获取一个 gRPC Stub
    void ReturnVerifyStub(std::unique_ptr<VerifyService::Stub> stub);  // 归还 gRPC Stub
    void Close();  // 关闭 gRPC Stub 池
private:
    VerifyStubPool();

    bool _is_shutdown;
    size_t _pool_size;
    std::queue<std::unique_ptr<VerifyService::Stub>> _stub_queue;
    std::mutex _mutex;
    std::condition_variable _cond_var;
};