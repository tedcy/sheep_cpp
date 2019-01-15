if [[ $protoc_path == "" ]];then
    protoc_path=`which protoc`
fi
if [[ $grpc_cpp_plugin_path == "" ]];then
    grpc_cpp_plugin_path=`which grpc_cpp_plugin`
fi
if [[ $protoc_path == "" ]];then
    exit -1
fi
$protoc_path -I. --grpc_out=../ --plugin=protoc-gen-grpc=$grpc_cpp_plugin_path helloworld.proto
$protoc_path -I. --cpp_out=../ helloworld.proto
