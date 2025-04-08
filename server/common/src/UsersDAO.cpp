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

