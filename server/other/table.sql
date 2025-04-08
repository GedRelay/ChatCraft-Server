create database chatcraft;

use chatcraft;


-- 创建表
create table users (
    id int auto_increment primary key,
    name varchar(255) not null,
    email varchar(255) not null,
    password varchar(255) not null
);


-- 插入测试数据
insert into users(name, email, password) 
values('ged1', '123456@qq.com', '123456');


-- 事务：注册用户
delimiter $$
create definer=`root`@`%` procedure reg_user(
    in user_name varchar(255),
    in user_email varchar(255),
    in user_password varchar(255),
    out result int
)
begin
    -- 如果遇到异常，则回滚事务
    declare exit handler for sqlexception
    begin
        rollback;
        set result = -1;
    end;

    -- 开始事务
    start transaction;

    -- 检查用户名是否已存在
    if exists (select `name` from `users` where `name` = user_name) then
        set result = 0;
        commit;
    else
        -- 检测邮箱是否已存在
        if exists (select `email` from `users` where `email` = user_email) then
            set result = 0;
            commit;
        else
            -- 插入新用户，返回用户id
            insert into users(`name`, `email`, `password`) 
            values(user_name, user_email, user_password);
            set result = last_insert_id();
            commit;
        end if;
    end if;
end$$
delimiter ;