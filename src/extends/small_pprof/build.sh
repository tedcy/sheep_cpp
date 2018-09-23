set -e

#build perftools
path=`find . -name gperftools-httpd`
if [[ $path == "" ]];then
    git clone https://github.com/tedcy/gperftools-httpd
fi

#test if ./bootstrap.sh and build.sh runed
cd gperftools-httpd
path=`find . -name libgperftools-httpd.a|head -n 1`
if [[ $path == "" ]];then
    ./bootstrap.sh
    ./build.sh
fi
cd -

#test if ./build.sh runed
path=`find build -name libsmall_pprof.a|head -n 1`
if [[ $path != "" ]];then
    exit 0
fi

#find .a libs
mkdir -pv build
cp gperftools-httpd/libstacktrace.a build
cp gperftools-httpd/build/libgperftools-httpd.a build
path=`find /usr/ -name libtcmalloc.a|head -n 1`
if [[ $path == "" ]];then
    echo "can't find libtcmalloc.a"
    exit -1
fi
cp $path build
path=`find /usr/ -name libprofiler.a|head -n 1`
if [[ $path == "" ]];then
    echo "can't find libprofiler.a"
    exit -1
fi
cp $path build

#combine .a libs into libsmall_pprof.a
rm -rf /tmp/small_pprof
mkdir -pv /tmp/small_pprof
cp ./build/libstacktrace.a ./build/libgperftools-httpd.a ./build/libtcmalloc.a ./build/libprofiler.a /tmp/small_pprof
cd /tmp/small_pprof
ar x libstacktrace.a
ar x libgperftools-httpd.a
ar x libtcmalloc.a
ar x libprofiler.a
ar cru libsmall_pprof.a *.o
ranlib libsmall_pprof.a
cd -
cp /tmp/small_pprof/libsmall_pprof.a ./build

#cp headers
cp gperftools-httpd/gperftools-httpd.h .
