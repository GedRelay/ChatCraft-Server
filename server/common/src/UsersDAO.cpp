#include "../include/UsersDAO.h"

UsersDAO::UsersDAO() {
    // Constructor implementation
}


UsersDAO::~UsersDAO() {
    // Destructor implementation
}


int UsersDAO::RegisterUser(std::unique_ptr<sql::Connection>& connection, const std::string& username, const std::string& email, const std::string& password) {
    // 若用户名或邮箱已存在，则返回0
    // 其他错误返回-1
    // 成功返回用户ID
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(connection->prepareStatement("call reg_user(?, ?, ?, @result)"));
        pstmt->setString(1, username);
        pstmt->setString(2, email);
        pstmt->setString(3, password);
        pstmt->execute();  // 执行存储过程
        // 获取结果
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("select @result as result"));
        if (res->next()) {
            int result = res->getInt("result");
            return result;  // 返回结果
        }
        return -1;  // 执行失败
    } catch (sql::SQLException& e) {
        std::cerr << "Error registering user: " << e.what() << std::endl;
        return -1;
    }
}


int UsersDAO::ExistsEmail(std::unique_ptr<sql::Connection>& connection, const std::string& email) {
    // 检查邮箱是否存在
    // 若存在，则返回1
    // 若不存在，则返回0
    // 其他错误返回-1
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(connection->prepareStatement("select email from users where email = ?"));
        pstmt->setString(1, email);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            std::string result = res->getString("email");
            if (result == email) {
                return 1;  // 邮箱存在
            }
        }
        return 0;  // 邮箱不存在
    } catch (sql::SQLException& e) {
        std::cerr << "Error checking email: " << e.what() << std::endl;
        return -1;
    }
}


int UsersDAO::CheckUserAndEmail(std::unique_ptr<sql::Connection>& connection, const std::string& username, const std::string& email) {
    // 检查用户名和邮箱是否匹配
    // 若匹配，则返回1
    // 若不匹配，则返回0
    // 若邮箱不存在，则返回-1
    // 其他错误返回-1
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(connection->prepareStatement("select name from users where email = ?"));
        pstmt->setString(1, email);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            std::string result = res->getString("name");
            if (result == username) {
                return 1;  // 用户名和邮箱匹配
            } else {
                return 0;  // 用户名和邮箱不匹配
            }
        }
        return -1;  // 邮箱不存在
    } catch (sql::SQLException& e) {
        std::cerr << "Error checking user and email: " << e.what() << std::endl;
        return -1;
    }
}


int UsersDAO::ResetPassword(std::unique_ptr<sql::Connection>& connection, const std::string& username, const std::string& email, const std::string& password) {
    // 重置密码
    // 若修改成功，则返回1
    // 若修改失败（0 Changed），则返回0
    // 若新密码和旧密码相同，则返回2
    // 其他错误返回-1
    try {
        // 检查新密码是否和旧密码相同
        std::unique_ptr<sql::PreparedStatement> checkPstmt(connection->prepareStatement("select password from users where name = ? and email = ?"));
        checkPstmt->setString(1, username);
        checkPstmt->setString(2, email);
        std::unique_ptr<sql::ResultSet> checkRes(checkPstmt->executeQuery());
        if (checkRes->next()) {
            std::string oldPassword = checkRes->getString("password");
            if (oldPassword == password) {
                return 2;  // 新密码和旧密码相同
            }
        }
        else {
            return 0;  // 用户名和邮箱不匹配
        }
        // 修改密码
        std::unique_ptr<sql::PreparedStatement> pstmt(connection->prepareStatement("update users set password = ? where name = ? and email = ?"));
        pstmt->setString(1, password);
        pstmt->setString(2, username);
        pstmt->setString(3, email);
        int result = pstmt->executeUpdate();
        if (result > 0) {
            return 1;  // 密码修改成功
        } else {
            return 0;  // 密码修改失败
        }
    } catch (sql::SQLException& e) {
        std::cerr << "Error resetting password: " << e.what() << std::endl;
        return -1;
    }
}