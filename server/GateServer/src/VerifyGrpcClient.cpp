#include "../include/VerifyGrpcClient.h"

VerifyGrpcClient::VerifyGrpcClient() {
    // 创建 gRPC 通道
    std::string verify_server_host = ConfigManager::GetConfigAs("VerifyServer", "host");
    std::string verify_server_port = ConfigManager::GetConfigAs("VerifyServer", "port");
    std::string verify_server_address = verify_server_host + ":" + verify_server_port;
    auto channel = grpc::CreateChannel(verify_server_address, grpc::InsecureChannelCredentials());
}


GetVerifyRsp VerifyGrpcClient::RegisterGetVerifyCode(const std::string& email) {
    ClientContext context;
    GetVerifyReq request;
    GetVerifyRsp response;
    request.set_module(message::Module::REGISTER);
    request.set_email(email);
    // 调用 gRPC 方法发送请求，并接收响应
    auto stub = GrpcStubPool::GetInstance()->GetVerifyStub();
    Status status = stub->GetVerifyCode(&context, request, &response);
    // 归还 gRPC Stub
    GrpcStubPool::GetInstance()->ReturnVerifyStub(std::move(stub));
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
    auto stub = GrpcStubPool::GetInstance()->GetVerifyStub();
    Status status = stub->GetVerifyCode(&context, request, &response);
    // 归还 gRPC Stub
    GrpcStubPool::GetInstance()->ReturnVerifyStub(std::move(stub));
    if (!status.ok()) {
        response.set_error(status.error_code());
        return response;
    }

    return response;
}