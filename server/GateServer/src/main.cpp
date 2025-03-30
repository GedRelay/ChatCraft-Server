#include "../include/CServer.h"
#include "../include/const.h"
#include <cstring>

int main(){
    try{
        std::string host = ConfigManager::GetConfigAs("GateServer", "host");
        unsigned short port = ConfigManager::GetConfigAs<unsigned short>("GateServer", "port");
        net::io_context io_context{ 1 };
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait(
            [&io_context](const boost::system::error_code & error, int signal_num){
                io_context.stop();
            }
        );

        std::shared_ptr<CServer> gate_server = std::make_shared<CServer>(io_context, host, port);
        gate_server->Start();
        std::cout << "Server started at " << host << ":" << port << std::endl;
        io_context.run();
        std::cout << "Server stopped" << std::endl;
    }
    catch(std::exception const& e){
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
