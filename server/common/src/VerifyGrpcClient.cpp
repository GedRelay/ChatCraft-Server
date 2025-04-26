#include "../include/VerifyGrpcClient.h"

VerifyGrpcClient::VerifyGrpcClient() {
    
}


GetVerifyRsp VerifyGrpcClient::RegisterGetVerifyCode(const std::string& email) {
    ClientContext context;
    GetVerifyReq request;
    GetVerifyRsp response;
    request.set_module(message::Module::REGISTER);
    request.set_email(email);
    // 调用 gRPC 方法发送请求，并接收响应
    auto stub = VerifyStubPool::GetInstance()->GetVerifyStub();
    Status status = stub->GetVerifyCode(&context, request, &response);
    // 归还 gRPC Stub
    VerifyStubPool::GetInstance()->ReturnVerifyStub(std::move(stub));
    if (!status.ok()) {
        response.set_error(status.error_code());
        return response;
    }

    return response;
}

GetVerifyRsp VerifyGrpcClient::ResetGetVerifyCode(const std::string& email) {
    ClientContext context;
    GetVerifyReq request;
    GetVerifyRsp response;
    request.set_module(message::Module::RESET);
    request.set_email(email);
    // 调用 gRPC 方法发送请求，并接收响应
    auto stub = VerifyStubPool::GetInstance()->GetVerifyStub();
    Status status = stub->GetVerifyCode(&context, request, &response);
    // 归还 gRPC Stub
    VerifyStubPool::GetInstance()->ReturnVerifyStub(std::move(stub));
    if (!status.ok()) {
        response.set_error(status.error_code());
        return response;
    }

    return response;
}