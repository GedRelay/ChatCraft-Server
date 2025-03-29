#include "../include/CServer.h"
#include "../include/const.h"
#include <cstring>

int main(){
    try{
        // unsigned short port = stoi((*ConfigManager::GetInstance())["GateServer"]["port"]);
        unsigned short port = ConfigManager::GetConfigAs<unsigned short>("GateServer", "port");
        net::io_context io_context{ 1 };
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait(
            [&io_context](const boost::system::error_code & error, int signal_num){
                io_context.stop();
            }
        );

        std::make_shared<CServer>(io_context, port)->Start();
        std::cout << "Server started at port " << port << std::endl;
        io_context.run();
        std::cout << "Server stopped" << std::endl;
    }
    catch(std::exception const& e){
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
