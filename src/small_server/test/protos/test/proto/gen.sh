protoc -I. --grpc_out=../ --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` helloworld.proto
protoc -I. --cpp_out=../ helloworld.proto
#protoc -I. --grpc_out=../cpp --plugin=protoc-gen-grpc="/mnt/sda3/root/vcpkg/packages/grpc_x64-linux/bin/grpc_cpp_plugin" company.proto
#protoc -I. --cpp_out=../cpp company.proto
