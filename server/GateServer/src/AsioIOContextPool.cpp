#include "../include/AsioIOContextPool.h"


AsioIOContextPool::AsioIOContextPool():
    _pool_size(CONST::IO_CONTEXT_POOL_SIZE) {
    _io_contexts = std::vector<net::io_context>(_pool_size);
    for (size_t i = 0; i < _pool_size; ++i) {
        _work_guards.emplace_back(net::make_work_guard(_io_contexts[i].get_executor()));
        _threads.emplace_back([this, i]() {
            _io_contexts[i].run();
        });
    }
}


AsioIOContextPool::~AsioIOContextPool(){
    Stop();
}


net::io_context& AsioIOContextPool::GetIOContext(){
    std::lock_guard<std::mutex> lock(_mutex);
    net::io_context& io_context = _io_contexts[_index];
    _index = (_index + 1) % _pool_size;
    return io_context;
}


void AsioIOContextPool::Stop(){
    for(auto &io_context : _io_contexts){
        io_context.stop();
    }
    _work_guards.clear();
    for (auto& t : _threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}


