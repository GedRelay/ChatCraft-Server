#include "../include/HttpConnection.h"
#include "../include/LogicSystem.h"

HttpConnection::HttpConnection(tcp::socket socket): 
    _socket(std::move(socket)){
}


void HttpConnection::Start(){
    http::async_read(
        _socket,
        _buffer,
        _request,
        [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred){
            try{
                if(!ec){
                    boost::ignore_unused(bytes_transferred);
                    self->HandleRequest();
                    self->CheckDeadline();
                }
                else{
                    std::cerr << "Error: " << ec.message() << std::endl;
                }
            }
            catch (std::exception const& e){
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    );
}


void HttpConnection::HandleRequest(){
    _response.version(_request.version());
    _response.keep_alive(false);
    if(_request.method() == http::verb::get){  // GET请求
        bool success = LogicSystem::GetInstance()->HandleGet(_request, _response);  // 处理GET请求
        WriteResponse();
    }
    else if(_request.method() == http::verb::post){  // POST请求
        bool success = LogicSystem::GetInstance()->HandlePost(_request, _response);  // 处理POST请求
        WriteResponse();
    }
}


void HttpConnection::WriteResponse(){
    http::async_write(
        _socket,
        _response,
        [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred){
            self->_socket.shutdown(tcp::socket::shutdown_send, ec);
            self->_deadline.cancel();
        }
    );
}


void HttpConnection::CheckDeadline(){
    _deadline.async_wait(
        [self = shared_from_this()](boost::system::error_code ec){
            if(!ec){
                self->_socket.close(ec);
            }
        }
    );
}