#pragma once
#include "const.h"
#include "CSession.h"


class CSession;
class CServer{
public:
    CServer(net::io_context& io_context, unsigned short port);
    void ClearSession(const std::string& session_id);

private:
    void WaitAccept();
    std::string GenerateSessionId();

    net::ip::tcp::acceptor _acceptor;
    net::io_context& _io_context;
    std::map<std::string, std::shared_ptr<CSession>> _sessions;
};