#include "../include/VerifyGrpcClient.h"

VerifyGrpcClient::VerifyGrpcClient() {
    // 创建 gRPC 通道
    auto channel = grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials());
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