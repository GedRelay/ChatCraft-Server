#pragma once
#include "../include/const.h"
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>

class UsersDAO {
public:
    UsersDAO();
    ~UsersDAO();

    int RegisterUser(std::unique_ptr<sql::Connection>& connection, const std::string& username, const std::string& email, const std::string& password);
    int ExistsEmail(std::unique_ptr<sql::Connection>& connection, const std::string& email);
    int CheckUserAndEmail(std::unique_ptr<sql::Connection>& connection, const std::string& username, const std::string& email);
    int ResetPassword(std::unique_ptr<sql::Connection>& connection, const std::string& username, const std::string& email, const std::string& password);
private:
};