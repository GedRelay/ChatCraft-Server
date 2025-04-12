
GateServer
使用asio监听http连接，通过解析http请求，获取请求的url和参数
根据url和参数，调用对应的处理函数
创建线程池，通过线程池处理请求，以提高性能


VerifyServer
使用 nodejs 搭建 VerifyServer
使用 uuid 生成验证码
使用 nodemailer 发送邮件


StatusServer
使用 c++ 搭建 StatusServer
为传入的uid生成随机token，以及为其分配ChatServer


服务器之间使用 gRPC 进行通信，为了提高性能，服务器采用线程池处理gRPC请求
使用 hiredis 实现服务器与redis的通信，为了提高性能，使用redis连接池
使用 mysqlcppconn 实现服务器与mysql的通信，为了提高性能，使用mysql连接池


注册流程：
用户在客户端输入邮箱后首先发送验证码请求到GateServer（post,/get_verify_code）
GateServer 解析请求，调用 VerifyServer 发送用于注册的验证码邮件
同时在redis中存储验证码和过期时间
用户在客户端输入注册信息以及验证码后发送注册请求（密码加密）到GateServer（post, /user_register）
GateServer 解析注册信息合法性以及验证码合法性
如果合法，调用数据库创建用户，同时删除redis中的验证码


修改密码流程：
用户在客户端输入用户名和邮箱后首先发送验证码请求到GateServer（post,/reset_get_verify_code）
GateServer 解析请求，查询数据库中用户名和邮箱是否存在和是否匹配
如果合法，则调用 VerifyServer 发送用于修改密码的验证码邮件
同时在redis中存储验证码和过期时间
用户在客户端填写修改密码的信息以及验证码后发送修改密码请求（密码加密）到GateServer（post, /reset_password）
GateServer 解析信息合法性以及验证码合法性
如果合法，调用数据库修改用户密码，同时删除redis中的验证码


Client和GateServer之间使用http协议进行通信
Client通过提交POST请求和JSON格式的数据来与GateServer进行交互
1. 如果url不存在，则GateServer返回文本"url not found"
2. 如果Json解析失败，返回{"status":-1, "msg":"json parse error"}
3. 如果出现其他错误，返回{"status":-1, "msg":"错误描述"}
4. 如果请求成功，返回{"status":0, "msg":"成功信息", 其他数据字段}