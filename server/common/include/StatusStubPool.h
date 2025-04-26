#pragma once
#include "const.h"
#include <grpcpp/grpcpp.h>
#include "proto/message.grpc.pb.h"

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

class StatusStubPool : public Singleton<StatusStubPool> {
    friend class Singleton<StatusStubPool>;
public:
    ~StatusStubPool();
    std::unique_ptr<StatusService::Stub> GetStatusStub();  // 获取一个 gRPC Stub
    void ReturnStatusStub(std::unique_ptr<StatusService::Stub> stub);  // 归还 gRPC Stub
    void Close();  // 关闭 gRPC Stub 池

private:
    StatusStubPool();

    bool _is_shutdown;
    size_t _pool_size;
    std::queue<std::unique_ptr<StatusService::Stub>> _stub_queue;
    std::mutex _mutex;
    std::condition_variable _cond_var;
};
