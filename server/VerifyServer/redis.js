// 用来封装redis操作

const Redis = require('ioredis');
const config_module = require("./config");

// 创建Redis连接
const RedisCli = new Redis({
    host: config_module.redis_host,
    port: config_module.redis_port,
    password: config_module.redis_password
});
RedisCli.on("error", function (err) {
    console.error("RedisCli connect error");
    RedisCli.quit();
});


// 查询key是否存在
// 出现错误时返回null
async function RedisQuery(key) {
    try{
        const result = await RedisCli.exists(key);
        if(result === 0){
            console.error(`Redis key ${key} not found`);
            return null;
        }
        console.log(`Redis key ${key} found: ${result}`);
        return result;
    }
    catch(error) {
        console.error(`Error querying Redis key ${key}:`, error);
        return null;
    }
}



// 根据key获取value
// 出现错误时返回null
async function RedisGet(key) {
    try{
        const result = await RedisCli.get(key);
        if(result === null){
            console.error(`Redis key ${key} not found`);
            return null;
        }
        console.log(`Redis key ${key} found: ${result}`);
        return result;
    }
    catch (error) {
        console.error(`Error getting Redis key ${key}:`, error);
        return null;
    }
}


// 设置key和value，以及过期时间（单位：秒）
// 出现错误时返回false，成功时返回true
async function RedisSetExpire(key, value, expire) {
    try{
        await RedisCli.set(key, value);
        await RedisCli.expire(key, expire);
        console.log(`Redis key ${key} set with expiration ${expire}`);
        return true;
    }
    catch (error) {
        console.error(`Error setting Redis key ${key}:`, error);
        return false;
    }
}

// 关闭Redis连接
function RedisQuit() {
    RedisCli.quit();
}


module.exports = {
    RedisQuery,
    RedisGet,
    RedisSetExpire,
    RedisQuit
};