set -e

#test if build.sh runned
path=`find build -maxdepth 1 -name libprotobuf.a`
if [[ $path != "" ]];then
    exit 0
fi

path=""
path=`find . -name grpc`
if [[ $path == "" ]];then
    git clone https://github.com/grpc/grpc
    cd grpc
    git checkout v1.17.2
    git submodule update --init
    make -j8
    cd -
fi

rm -rf build
mkdir -pv build
cp grpc/libs/opt/libgrpc++.a build/
cp grpc/libs/opt/libgrpc.a build/
cp grpc/libs/opt/protobuf/libprotobuf.a build/
cp ./grpc/bins/opt/protobuf/protoc build/
cp ./grpc/bins/opt/grpc_cpp_plugin build/
