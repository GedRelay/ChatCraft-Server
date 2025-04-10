#include "../include/MysqlManager.h"

// MySQL管理器初始化
MysqlManager::MysqlManager() {
    Init();
}
void MysqlManager::Init() {
    std::string host = ConfigManager::GetConfigAs("MySQL", "host");
    std::string port = ConfigManager::GetConfigAs("MySQL", "port");
    std::string user = ConfigManager::GetConfigAs("MySQL", "user");
    std::string password = ConfigManager::GetConfigAs("MySQL", "pass");
    std::string database = ConfigManager::GetConfigAs("MySQL", "schema");
    std::string address = host + ":" + port;
    _mysql_connection_pool = std::make_unique<MysqlConnectionPool>(address, user, password, database, CONST::MYSQL_CONNECTION_POOL_SIZE);
    _users_dao = std::make_unique<UsersDAO>();
}


// MySQL管理器析构函数
MysqlManager::~MysqlManager() {
    // Destructor implementation
}

// 初始化MySQL连接池
MysqlConnectionPool::MysqlConnectionPool(const std::string& address, const std::string& user, const std::string& password, const std::string& database, size_t pool_size):
    _pool_size(pool_size) {
    try{
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        for (size_t i = 0; i < pool_size; ++i) {
            std::unique_ptr<sql::Connection> connection(driver->connect(address, user, password));
            connection->setSchema(database);
            _mysql_connections.push(std::move(connection));
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error connecting to MySQL: " << e.what() << std::endl;
        throw;
    }
}


// 连接池析构函数
MysqlConnectionPool::~MysqlConnectionPool() {
    Close();
}


// 获取MySQL连接
std::unique_ptr<sql::Connection> MysqlConnectionPool::GetConnection() {
    std::unique_lock<std::mutex> lock(_mutex);
    while (_mysql_connections.empty() && !_is_shutdown) {
        _cond.wait(lock);
    }
    if (_is_shutdown) {
        return nullptr;
    }
    std::unique_ptr<sql::Connection> connection = std::move(_mysql_connections.front());
    _mysql_connections.pop();
    return connection;
}


// 归还MySQL连接
void MysqlConnectionPool::ReturnConnection(std::unique_ptr<sql::Connection> connection) {
    std::unique_lock<std::mutex> lock(_mutex);
    _mysql_connections.push(std::move(connection));
    _cond.notify_one();
}


// 关闭连接池
void MysqlConnectionPool::Close() {
    _is_shutdown = true;
    _cond.notify_all();
    // 等待所有MySQL连接被归还，即连接池大小为_pool_size
    std::unique_lock<std::mutex> lock(_mutex);
    while (_mysql_connections.size() < _pool_size) {
        _cond.wait(lock);
    }
    // 释放所有MySQL连接
    while (!_mysql_connections.empty()) {
        _mysql_connections.pop();
    }
    _pool_size = 0;
}


// 注册用户, -1表示创建失败, 0表示用户已存在, 成功返回用户ID
int MysqlManager::RegisterUser(const std::string& username, const std::string& email, const std::string& password) {
    // 获取MySQL连接
    std::unique_ptr<sql::Connection> connection = _mysql_connection_pool->GetConnection();
    if (connection == nullptr) {
        return -1;  // 连接池已关闭
    }
    // 创建用户
    int result = _users_dao->RegisterUser(connection, username, email, password);
    _mysql_connection_pool->ReturnConnection(std::move(connection));
    return result;
}


// 检查邮箱是否存在
int MysqlManager::ExistsEmail(const std::string& email) {
    // 获取MySQL连接
    std::unique_ptr<sql::Connection> connection = _mysql_connection_pool->GetConnection();
    if (connection == nullptr) {
        return -1;  // 连接池已关闭
    }
    // 检查邮箱是否存在
    int result = _users_dao->ExistsEmail(connection, email);
    _mysql_connection_pool->ReturnConnection(std::move(connection));
    return result;
}


// 检查用户名和邮箱是否匹配
int MysqlManager::CheckUserAndEmail(const std::string& username, const std::string& email) {
    // 获取MySQL连接
    std::unique_ptr<sql::Connection> connection = _mysql_connection_pool->GetConnection();
    if (connection == nullptr) {
        return -1;  // 连接池已关闭
    }
    // 检查用户名和邮箱是否匹配
    int result = _users_dao->CheckUserAndEmail(connection, username, email);
    _mysql_connection_pool->ReturnConnection(std::move(connection));
    return result;
}


// 重置密码
int MysqlManager::ResetPassword(const std::string& username, const std::string& email, const std::string& password) {
    // 获取MySQL连接
    std::unique_ptr<sql::Connection> connection = _mysql_connection_pool->GetConnection();
    if (connection == nullptr) {
        return -1;  // 连接池已关闭
    }
    // 重置密码
    int result = _users_dao->ResetPassword(connection, username, email, password);
    _mysql_connection_pool->ReturnConnection(std::move(connection));
    return result;
}