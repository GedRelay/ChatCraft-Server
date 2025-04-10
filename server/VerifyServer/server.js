// 服务器入口

const grpc = require("@grpc/grpc-js");
const { v4: uuidv4 } = require("uuid");
const message_proto = require("./proto");
const config_module = require("./config");
const email_module = require("./email");
const const_module = require("./const");
const redis_module = require("./redis");


function GenEmail(module, email, verifycode) {
    let mail_options = {
        from: config_module.email_user,
        to: email,
        subject: 'ChatCraft验证码',
        text: `您的验证码是：${verifycode} ，有效期为3分钟。`
    };
    if(module == "REGISTER") {
        mail_options.subject = 'ChatCraft注册验证码';
        mail_options.text = `欢迎您注册ChatCraft! 您的验证码是：${verifycode} ，有效期为3分钟。`
    }
    else if(module == "RESET") {
        mail_options.subject = 'ChatCraft重置密码验证码';
        mail_options.text = `您正在重置ChatCraft的密码，您的验证码是：${verifycode} ，有效期为3分钟。`
    }
    return mail_options;
}

async function GenVerifyCode(module, email) {
    let key = email;
    if (module == "REGISTER") {
        key = config_module.verifycode_prefix.REGISTER + email;
    }
    else if (module == "RESET") {
        key = config_module.verifycode_prefix.RESET + email;
    }
    let verifycode = await redis_module.RedisGet(key);
    if (verifycode == null) { // 如果不存在，则生成新的验证码
        uuid = uuidv4();
        verifycode = uuid.slice(0, 4);
        let set_res = await redis_module.RedisSetExpire(key, verifycode, 180);
        if (set_res == false) {
            console.error("Redis set error");
            return null;
        }
    }
    return verifycode;
}


async function GetVerifyCode(call, callback) {
    console.log("module:", call.request.module);
    console.log("email:", call.request.email);
    try{
        let verifycode = await GenVerifyCode(call.request.module, call.request.email);
        if (verifycode == null) {
            callback(null, {
                email: call.request.email,
                error: const_module.Errors.RedisErr
            });
            return;
        }
        let mail_options = GenEmail(call.request.module, call.request.email, verifycode);
        console.log("mail_options:", mail_options);

        // 发送邮件
        let send_res = await email_module.sendMail(mail_options);
        console.log("send_res:", send_res);

        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Success,
            code: verifycode
        });

    }
    catch (err) {
        console.error("Error in GetVerifyCode:", err);
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Exception
        });
    }
}


function main() {
    var server = new grpc.Server();
    server.addService(message_proto.VerifyService.service, {GetVerifyCode: GetVerifyCode});
    const address = `${config_module.verify_server_host}:${config_module.verify_server_port}`;
    server.bindAsync(address, grpc.ServerCredentials.createInsecure(), () => {
        console.log(`VerifyServer is running at ${address}`);
    });
}

main();