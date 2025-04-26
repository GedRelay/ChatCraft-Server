#include "const.h"
#include "../include/CServer.h"


int main(int argc, char* argv[]){
    try{
        int server_id = 1;
        if(argc == 2 && std::strcmp(argv[1], "1") == 0) server_id = 1;
        else if(argc == 2 && std::strcmp(argv[1], "2") == 0) server_id = 2;
        unsigned short port = ConfigManager::GetConfigAs<unsigned short>("ChatServer1", "port");
        std::string host = ConfigManager::GetConfigAs("ChatServer1", "host");
        if(server_id == 2){
            port = ConfigManager::GetConfigAs<unsigned short>("ChatServer2", "port");
            host = ConfigManager::GetConfigAs("ChatServer2", "host");
        }

        net::io_context io_context{ 1 };
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait(
            [&io_context](const boost::system::error_code & error, int signal_num){
                io_context.stop();
            }
        );

        std::shared_ptr<CServer> chat_server = std::make_shared<CServer>(io_context, port);
        std::cout << "ChatServer " << server_id << " started at " << host << ":" << port << std::endl;
        io_context.run();
        std::cout << "ChatServer " << server_id << " stopped" << std::endl;
    }
    catch(std::exception const& e){
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}