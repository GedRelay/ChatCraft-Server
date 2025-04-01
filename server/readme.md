
GateServer
使用asio监听http连接，通过解析http请求，获取请求的url和参数
根据url和参数，调用对应的处理函数
创建线程池，通过线程池处理请求，以提高性能


VerifyServer
使用 nodejs 搭建 VerifyServer
使用 uuid 生成验证码
使用 nodemailer 发送邮件


服务器之间使用 gRPC 进行通信，为了提高性能，服务器采用线程池处理gRPC请求
使用 hiredis 实现服务器与redis的通信，为了提高性能，使用redis连接池


注册流程：
用户在客户端输入邮箱后首先发送验证码请求到GateServer
GateServer 解析请求，调用 VerifyServer 发送验证码邮件
同时在redis中存储验证码和过期时间
用户在客户端输入注册信息以及验证码后发送注册请求到GateServer
GateServer 解析注册信息合法性以及验证码合法性
如果合法，调用数据库创建用户