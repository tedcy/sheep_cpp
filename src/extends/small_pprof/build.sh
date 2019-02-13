set -e
set -x

ifMac=`uname -a|grep -o Mac|head -n 1`
sys=`cat /proc/version|grep -o "Ubuntu\|Red Hat"|head -n 1`

mkdir -pv build

#build unwind
#FIXME: build by source code lead to hang program, I can't fix it
#path=`find ./build -name libunwind-x86_64.a`
#if [[ $path == "" ]];then
#    path=`find . -name libunwind-1.3.1.tar.gz`
#    if [[ $path == "" ]];then
#        wget https://github.com/libunwind/libunwind/releases/download/v1.3.1/libunwind-1.3.1.tar.gz
#    fi
#    rm -rf libunwind-1.3.1
#    tar xvf libunwind-1.3.1.tar.gz
#    cd libunwind-1.3.1
#    autoreconf -i
#    ./configure
#    cp /usr/bin/libtool ./libtool
#    make -j8
#    cp src/.libs/libunwind-x86_64.a ../build
#    cd -
#fi
if [[ $sys == "Ubuntu" ]];then
    apt-get install -y liblzma-dev libunwind8-dev
fi
if [[ $sys == "Red Hat" ]];then
    yum install -y lzma-dev libunwind
fi

#build google-perftools
path=`find ./build -name libtcmalloc_and_profiler.a`
if [[ $path == "" ]];then
    path=`find . -name gperftools-2.7.tar.gz`
    if [[ $path == "" ]];then
        wget https://github.com/gperftools/gperftools/releases/download/gperftools-2.7/gperftools-2.7.tar.gz
    fi
    rm -rf gperftools-2.7
    tar xvf gperftools-2.7.tar.gz
    cd gperftools-2.7
    autoreconf -i
    ./configure
    make -j8
    cp .libs/libtcmalloc_and_profiler.a ../build
    cd -
fi

#build perftools
path=`find ./build -name libgperftools-httpd.a`
if [[ $path == "" ]];then
    path=`find . -name gperftools-httpd`
    if [[ $path == "" ]];then
        git clone https://github.com/awesomeleo/gperftools-httpd
        cp ./CMakeLists_gperftools-httpd.txt gperftools-httpd/CMakeLists.txt
    fi
    cd gperftools-httpd
    mkdir -pv build
    cd build
    cmake ..
    make -j8
    cp libgperftools-httpd.a ../../build/
    cd ../..
fi

path=`find . -name FlameGraph` 
if [[ $path = "" ]];then
    git clone https://github.com/brendangregg/FlameGraph
fi
