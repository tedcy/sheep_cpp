## 自定义安装介绍
[c++性能分析gperftools总结](http://weakyon.com/2018/08/16/summarize-of-gperftools.html)

## 参考资料
https://gperftools.github.io/gperftools/pprof_remote_servers.html
https://blog.csdn.net/jhzhou/article/details/7245992
https://blog.csdn.net/cica0cica/article/details/76919381

## usage

```
cpu: 
pprof ./main http://localhost:9999/pprof/profile
heap:
pprof ./main http://localhost:9999/pprof/growth
```

## TODO List
heap profile run as cpu profile
