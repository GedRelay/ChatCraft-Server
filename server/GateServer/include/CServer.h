#pragma once
#include "const.h"
#include "HttpConnection.h"


class CServer: public std::enable_shared_from_this<CServer>{
public:
    CServer(net::io_context& io_context, std::string host, unsigned short port);
    void Start();
private:
    tcp::acceptor _acceptor;
    net::io_context& _io_context;
    tcp::socket _socket;
};