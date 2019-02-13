## 自定义安装介绍
[c++性能分析gperftools总结](http://weakyon.com/2018/08/16/summarize-of-gperftools.html)

## 参考资料
https://gperftools.github.io/gperftools/pprof_remote_servers.html

https://blog.csdn.net/jhzhou/article/details/7245992

https://blog.csdn.net/cica0cica/article/details/76919381

## usage

normal graph

```
cpu: 
gperftools-2.7/src/pprof ./main http://localhost:9999/pprof/profile
heap growth(alloc most, not means memory growth):
gperftools-2.7/src/pprof ./main http://localhost:9999/pprof/growth
heap status(run program with env TCMALLOC_SAMPLE_PARAMETER=524288 first)
gperftools-2.7/src/pprof ./main http://localhost:9999/pprof/heap
```

flame graph

```
cpu:
gperftools-2.7/src/pprof --collapsed ./main http://localhost:9999/pprof/profile > out.data
FlameGraph/flamegraph.pl out.data > /tmp/out.svg
growth, heap
...(as cpu)
```


## pprof\_helper.sh 

a easy way to get flame graph

```
install:
./install_helper.sh

usage:
pprof_helper.sh ./main http://localhost:9999/pprof/profile
```
