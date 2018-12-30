1 T RedisClient将成员智能指针暂存以避免onDone执行完毕后，RedisClient被销毁导致share\_ptr信息丢失，连接无法还回去  
需要检查其他代码是否存在这种风险
2 F RedisClient可能存在内存/连接泄露，需要排查
3 http\_client\_backup need implent chunk
4 http\_client\_backup doesn't saved lead to cored(must be debug with valgrind)
5 http\_client\_backup doesn't need to specify host, it's stupid
