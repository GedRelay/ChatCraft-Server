#pragma once
#include "../include/const.h"
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>
#include "../include/UsersDAO.h"

class MysqlConnectionPool{
public:
    MysqlConnectionPool(const std::string& address, const std::string& user, const std::string& password, const std::string& database, size_t pool_size);
    ~MysqlConnectionPool();
    std::unique_ptr<sql::Connection> GetConnection();  // 获取MySQL连接
    void ReturnConnection(std::unique_ptr<sql::Connection> connection);  // 归还MySQL连接
    void Close();  // 关闭连接池

private:
    std::atomic<bool> _is_shutdown;
    size_t _pool_size;
    std::queue<std::unique_ptr<sql::Connection>> _mysql_connections;
    std::mutex _mutex;
    std::condition_variable _cond;
};


class MysqlManager : public Singleton<MysqlManager> {
friend class Singleton<MysqlManager>;
public:
    ~MysqlManager();
    int RegisterUser(const std::string& username, const std::string& email, const std::string& password);
    int ExistsEmail(const std::string& email);
    int CheckUserAndEmail(const std::string& username, const std::string& email);
    int ResetPassword(const std::string& username, const std::string& email, const std::string& password);

private:
    MysqlManager();
    void Init();

    std::unique_ptr<MysqlConnectionPool> _mysql_connection_pool;
    std::unique_ptr<UsersDAO> _users_dao;
};
