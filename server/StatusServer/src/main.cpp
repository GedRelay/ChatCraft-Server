#include "const.h"
#include "../include/StatusServer.h"

void RunServer();

int main(){
    try{
        RunServer();
    }
    catch (const std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}


void RunServer(){
    StatusServer service;
    std::string server_address = ConfigManager::GetConfigAs("StatusServer", "host") + ":" + ConfigManager::GetConfigAs("StatusServer", "port");
    
    grpc::ServerBuilder builder;
    // 监听端口
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // 注册服务
    builder.RegisterService(&service);
    // 启动服务器
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "StatusServer listening on " << server_address << std::endl;

    // 监听信号，实现优雅退出
    boost::asio::io_context io_context;
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&](const boost::system::error_code& error, int signal_number) {
        if (!error) {
            std::cout << "Shutting down server..." << std::endl;
            server->Shutdown();
        }
    });

    std::thread([&io_context](){
        io_context.run();
    }).detach();

    // 等待服务器停止
    server->Wait();
    io_context.stop();
}