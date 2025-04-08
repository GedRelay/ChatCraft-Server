1. 启动Mysql docker
```bash
docker start ged_mysql
```

2. 启动Redis
```bash
redis-server /usr/local/redis/etc/redis.conf
```

3. 启动GateServer
```bash
bin/GateServer
```

4. 启动VerifyServer
```bash
cd VerifyServer
npm run server
```