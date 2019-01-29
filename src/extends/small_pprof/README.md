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
heap growth(alloc most, not means memory growth):
pprof ./main http://localhost:9999/pprof/growth
heap status(run program with env TCMALLOC_SAMPLE_PARAMETER=524288 first)
pprof ./main http://localhost:9999/pprof/heap
```
