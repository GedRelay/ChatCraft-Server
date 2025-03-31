#include "../include/CServer.h"

CServer::CServer(net::io_context& io_context, std::string host, unsigned short port): 
    _acceptor(io_context, tcp::endpoint(net::ip::make_address(host), port)),
    _io_context(io_context),
    _socket(io_context){
}

void CServer::Start(){
    net::io_context& io_context = AsioIOContextPool::GetInstance()->GetIOContext();
    std::shared_ptr<HttpConnection> new_connection = std::make_shared<HttpConnection>(io_context);
    _acceptor.async_accept(
        new_connection->GetSocket(),
        [self = shared_from_this(), new_connection](const boost::system::error_code& ec) {
            if(!ec){
                new_connection->Start();
            } else {
                std::cerr << "Accept error: " << ec.message() << std::endl;
            }
            self->Start();
        }
    );
}