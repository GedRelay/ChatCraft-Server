// 服务器入口

const grpc = require("@grpc/grpc-js");
const { v4: uuidv4 } = require("uuid");
const message_proto = require("./proto");
const config_module = require("./config");
const email_module = require("./email");
const const_module = require("./const");

async function GetVarifyCode(call, callback) {
    console.log("email:", call.request.email);
    try{
        uuid = uuidv4();
        let varify_code = uuid.slice(0, 4);
        let mail_options = {
            from: config_module.email_user,
            to: call.request.email,
            subject: 'ChatCraft注册验证码',
            text: `欢迎您注册ChatCraft! 您的验证码是：${varify_code} ，有效期为3分钟。`
        };
        console.log("mail_options:", mail_options);
        let send_res = await email_module.sendMail(mail_options);
        console.log("send_res:", send_res);

        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Success,
            code: varify_code
        });

    }
    catch (err) {
        console.error("Error in GetVarifyCode:", err);
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Exception
        });
    }
}


function main() {
    var server = new grpc.Server();
    server.addService(message_proto.VarifyService.service, {GetVarifyCode: GetVarifyCode});
    const address = `${config_module.verify_server_host}:${config_module.verify_server_port}`;
    server.bindAsync(address, grpc.ServerCredentials.createInsecure(), () => {
        console.log(`VerifyServer is running at ${address}`);
    });
}

main();