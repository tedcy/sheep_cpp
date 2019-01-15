set -e
cd ../small_grpc/build
path=`pwd`
cd -
cd test/protos/test/proto
protoc_path=$path"/protoc" grpc_cpp_plugin_path=$path"/grpc_cpp_plugin" ./gen.sh
cd -
