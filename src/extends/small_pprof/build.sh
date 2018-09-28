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
rm -rf build
mkdir -pv build
#cp gperftools-httpd/libstacktrace.a build
cp gperftools-httpd/build/libgperftools-httpd.a build
needLibs=("libtcmalloc.a" "libprofiler.a")
for v in ${needLibs[@]};do
    path=`find /usr/local -name $v|head -n 1`
    if [[ $path == "" ]];then
        echo "can't find $v"
        exit -1
    fi
    cp $path build
done

#nouse, combine is not a good idea for mac plaform
#insteaded of linking more libs with cmake

##combine .a libs into libsmall_pprof.a
##rm -rf /tmp/small_pprof
##mkdir -pv /tmp/small_pprof
##installLibs=("libstacktrace.a" "libgperftools-httpd.a" ${needLibs[@]}) 
##for v in ${installLibs[@]};do
##    cp ./build/$v /tmp/small_pprof
##done
##cd /tmp/small_pprof
##for v in ${installLibs[@]};do
##    ar x $v
##done
##ar cru libsmall_pprof.a *.o
##ranlib libsmall_pprof.a
##cd -
##cp /tmp/small_pprof/libsmall_pprof.a ./build

#cp headers
cp gperftools-httpd/gperftools-httpd.h .
