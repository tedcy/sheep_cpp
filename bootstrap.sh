#set -x
set -e

cd ~
rootpath=`pwd`
cd -

findpaths=($rootpath)

#pkg path
sheep_pkg_path=`pwd`

#vcpkg path
if [[ $vcpkg_path == "" ]];then
    for path in ${findpaths[@]};do 
        #find vcpkg in root dir, get the first one
        vcpkg_path_arr=($(find $path -maxdepth 5 -name "vcpkg"))
        if (( ${#vcpkg_path_arr[@]} != 0 ));then
            vcpkg_path=${vcpkg_path_arr[0]}
            break
        fi
    done
    if [[ $vcpkg_path == "" ]];then
        echo "can't found vcpkg, run vcpkg_path=xxx ./bootstrap.sh or add findpaths in bootstrap.sh"
        exit -1
    fi
fi

#cmake path

if [[ $cmake_path == "" ]];then
    cmake_path=`which cmake`
    if [[ $cmake_path == "" ]];then
        echo "can't found cmake, run cmake_path = xxx ./bootstrap.sh"
        exit -1
    fi
fi

echo "export sheep_pkg_path=\"$sheep_pkg_path\"" > init_tools/env.sh
echo "export protoc_path=\"$sheep_pkg_path/src/extends/small_grpc/grpc/bins/opt/protobuf/protoc\"" >> init_tools/env.sh
echo "export grpc_cpp_plugin_path=\"$sheep_pkg_path/src/extends/small_grpc/grpc/bins/opt/grpc_cpp_plugin\"" >> init_tools/env.sh
echo "export vcpkg_path=\"$vcpkg_path\"" >> init_tools/env.sh
echo "export cmake_path=\"$cmake_path\"" >> init_tools/env.sh
if [[ $1 == "debug" ]];then
    echo "export build_type=\"DEBUG\"" >> init_tools/env.sh
else
    echo "export build_type=\"RELEASE\"" >> init_tools/env.sh
fi
echo "export load_env=\"true\"" >> init_tools/env.sh

cd init_tools
./build_all.sh $1
cd -
