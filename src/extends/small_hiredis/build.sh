set -e

#test if build.sh runned
path=`find . -name libsmall_hiredis.a`
if [[ $path != "" ]];then
    exit 0
fi

path=""
path=`find . -name hiredis`
if [[ $path == "" ]];then
    git clone https://github.com/redis/hiredis
    cd hiredis
    make
    cd -
fi

rm -rf build
mkdir -pv build
cp hiredis/libhiredis.a build/libsmall_hiredis.a
