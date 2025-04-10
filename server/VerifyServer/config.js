// 读取smtp.json文件中的邮箱配置，以及其他配置

const fs = require('fs');
const ini = require('ini');

// 读取smtp.json文件
let config = JSON.parse(fs.readFileSync('./smtp.json', 'utf8'));
let email_user = config.email.user_163;
let email_pass = config.email.pass_163;

// 读取config.ini文件
let config_ini = fs.readFileSync('../common/config.ini', 'utf8');
let config_ini_obj = ini.parse(config_ini);
let verify_server_host = config_ini_obj.VerifyServer.host;
let verify_server_port = config_ini_obj.VerifyServer.port;
let redis_host = config_ini_obj.Redis.host;
let redis_port = config_ini_obj.Redis.port;
let redis_password = config_ini_obj.Redis.pass;

let verifycode_prefix = {
    REGISTER: config_ini_obj.Prefix.register_verifycode_prefix,
    RESET: config_ini_obj.Prefix.reset_verifycode_prefix
};


module.exports = { email_user, email_pass, verify_server_host, verify_server_port, redis_host, redis_port, redis_password, verifycode_prefix };