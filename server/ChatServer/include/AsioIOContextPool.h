#pragma once
#include "const.h"



class AsioIOContextPool: public Singleton<AsioIOContextPool> {
    friend class Singleton<AsioIOContextPool>;
    using WorkGuard = net::executor_work_guard<net::io_context::executor_type>;
public:
    ~AsioIOContextPool();
    net::io_context& GetIOContext();
    void Stop();
    
private:
    AsioIOContextPool();
    std::vector<std::thread> _threads;
    std::vector<net::io_context> _io_contexts;
    std::vector<WorkGuard> _work_guards;
    size_t _pool_size;
    size_t _next_idx;
    std::mutex _mutex;
};