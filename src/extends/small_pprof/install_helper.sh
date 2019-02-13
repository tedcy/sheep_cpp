path=`pwd`
pprof_path=$path/gperftools-2.7/src/pprof
flamegraph_path=$path/FlameGraph/flamegraph.pl
echo "$pprof_path --collapsed \$1 \$2 > /tmp/out.data && $flamegraph_path /tmp/out.data > /tmp/out.svg && echo \"save to /tmp/out.svg\"" > /usr/local/bin/pprof_helper.sh
chmod +x /usr/local/bin/pprof_helper.sh
