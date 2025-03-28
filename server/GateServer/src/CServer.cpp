#include "../include/CServer.h"

CServer::CServer(net::io_context& io_context, unsigned short port): 
    _acceptor(io_context, tcp::endpoint(tcp::v4(), port)),
    _io_context(io_context),
    _socket(io_context){
}

void CServer::Start(){
    _acceptor.async_accept(
        _socket,
        [self = shared_from_this()](boost::system::error_code ec){
            if(!ec){
                std::make_shared<HttpConnection>(std::move(self->_socket))->Start();
            }
            self->Start();
        }
    );
}