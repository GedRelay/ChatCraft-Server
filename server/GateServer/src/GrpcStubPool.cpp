#include "../include/GrpcStubPool.h"


GrpcStubPool::GrpcStubPool():
    _pool_size(CONST::GRPC_STUB_POOL_SIZE),
    _is_shutdown(false) {
    std::string verify_server_host = ConfigManager::GetConfigAs("VerifyServer", "host");
    std::string verify_server_port = ConfigManager::GetConfigAs("VerifyServer", "port");
    std::string verify_server_address = verify_server_host + ":" + verify_server_port;
    for (size_t i = 0; i < _pool_size; ++i) {
        auto channel = grpc::CreateChannel(verify_server_address, grpc::InsecureChannelCredentials());
        _stub_queue.push(VarifyService::NewStub(channel));
    }
}


GrpcStubPool::~GrpcStubPool(){
    std::lock_guard<std::mutex> lock(_mutex);
    Close();
    while (!_stub_queue.empty()) {
        _stub_queue.pop();
    }
}


std::unique_ptr<VarifyService::Stub> GrpcStubPool::GetVerifyStub(){
    std::unique_lock<std::mutex> lock(_mutex);
    while (_stub_queue.empty() && !_is_shutdown) {
        _cond_var.wait(lock);
    }
    if (_is_shutdown) {  // 如果池已经关闭，返回 nullptr
        return nullptr;
    }
    auto stub = std::move(_stub_queue.front());
    _stub_queue.pop();
    return stub;
}


void GrpcStubPool::ReturnVerifyStub(std::unique_ptr<VarifyService::Stub> stub){
    std::lock_guard<std::mutex> lock(_mutex);
    _stub_queue.push(std::move(stub));
    _cond_var.notify_one();
}


void GrpcStubPool::Close(){
    _is_shutdown = true;
    _cond_var.notify_all();
}