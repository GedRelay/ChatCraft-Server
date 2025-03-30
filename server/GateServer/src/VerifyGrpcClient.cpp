#include "../include/VerifyGrpcClient.h"

VerifyGrpcClient::VerifyGrpcClient() {
    // 创建 gRPC 通道
    std::string verify_server_host = ConfigManager::GetConfigAs("VerifyServer", "host");
    std::string verify_server_port = ConfigManager::GetConfigAs("VerifyServer", "port");
    std::string verify_server_address = verify_server_host + ":" + verify_server_port;
    auto channel = grpc::CreateChannel(verify_server_address, grpc::InsecureChannelCredentials());
    // 创建 stub
    _stub = VarifyService::NewStub(channel);
}


GetVarifyRsp VerifyGrpcClient::GetVarifyCode(const std::string& email) {
    ClientContext context;
    GetVarifyReq request;
    GetVarifyRsp response;
    request.set_email(email);
    // 调用 gRPC 方法发送请求，并接收响应
    Status status = _stub->GetVarifyCode(&context, request, &response);
    if (!status.ok()) {
        response.set_error(status.error_code());
        return response;
    }
    return response;
}