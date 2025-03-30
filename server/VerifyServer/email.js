// 用来发送邮件

const nodemailer = require('nodemailer');
const config_module = require('./config');

// 创建一个SMTP客户端配置
let transporter = nodemailer.createTransport({
    host: 'smtp.163.com',
    port: 465,
    secure: true,
    auth: {
        user: config_module.email_user,
        pass: config_module.email_pass
    }
});

// 发送邮件
// @params mailOptions 邮件选项, 包含from, to, subject, text
function sendMail(mailOptions) {
    return new Promise((resolve, reject) => {
        transporter.sendMail(mailOptions, (error, info) => {
            if (error) {
                console.error('邮件发送失败:', error);
                reject(error);
            } else {
                console.log('邮件已发送成功:', info.response);
                resolve(info.response);
            }
        });
    });
}

module.exports = { sendMail };