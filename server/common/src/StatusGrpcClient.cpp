#include "../include/StatusGrpcClient.h"

StatusGrpcClient::StatusGrpcClient() {
    
}

GetChatServerRsp StatusGrpcClient::GetChatServer(const int& uid) {
    ClientContext context;
    GetChatServerReq request;
    GetChatServerRsp response;
    request.set_uid(uid);
    // 调用 gRPC 方法发送请求，并接收响应
    auto stub = StatusStubPool::GetInstance()->GetStatusStub();
    Status status = stub->GetChatServer(&context, request, &response);
    // 归还 gRPC Stub
    StatusStubPool::GetInstance()->ReturnStatusStub(std::move(stub));
    if (!status.ok()) {
        response.set_error(status.error_code());
        return response;
    }

    return response;
}


LoginRsp StatusGrpcClient::Login(const int& uid, const std::string& token){
    ClientContext context;
    LoginReq request;
    LoginRsp response;
    request.set_uid(uid);
    request.set_token(token);
    // 调用 gRPC 方法发送请求，并接收响应
    auto stub = StatusStubPool::GetInstance()->GetStatusStub();
    Status status = stub->Login(&context, request, &response);
    // 归还 gRPC Stub
    StatusStubPool::GetInstance()->ReturnStatusStub(std::move(stub));
    if (!status.ok()) {
        response.set_error(status.error_code());
        return response;
    }

    return response;
}