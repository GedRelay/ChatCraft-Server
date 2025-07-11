#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include "CServer.h"
#include "CMessageNode.h"
#include <queue>
#include "CLogicSystem.h"


class CServer;
class CSession: public std::enable_shared_from_this<CSession>{  // 继承自enable_shared_from_this，用于在回调函数中获取this的shared_ptr
public:
    CSession(std::shared_ptr<net::ip::tcp::socket> socket, std::string session_id, CServer *server);
    ~CSession();
    void Start();
    void SendMsg(uint16_t msg_id, char *msg, uint16_t msg_len);  // 发送消息
    void SendMsg(uint16_t msg_id, std::string msg_str);  // 发送消息
    std::string GetSessionId(bool full = false);
    
private:
    void HandleReadHead(const boost::system::error_code &error, size_t recv_pack_size, std::shared_ptr<CSession> self_ptr);  // 读取信息头的回调函数
    void HandleReadMsg(const boost::system::error_code &error, size_t recv_pack_size, std::shared_ptr<CSession> self_ptr);  //读取消息内容的回调函数
    void HandleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> self_ptr);  // 写操作完成后的回调函数
    
    std::shared_ptr<net::ip::tcp::socket> _socket;
    char _recv_pack_data[CONST::MSG_PACK_MAX_SIZE_BYTE];  // 接收消息的缓冲区
    CServer* _server;  // 保存server指针，用于清除session
    std::string _session_id;  // 该session的id
    std::queue<std::shared_ptr<CSendMessageNode>> _sending_msg_queue;  // 发送消息队列
    std::mutex _msg_queue_mutex;  // 用于保护消息队列的互斥锁
    std::shared_ptr<CRecvMessageNode> _cur_recv_msg;  // 当前正在接收的消息
};