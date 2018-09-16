#set -x
set -e

#pkg path
sheep_pkg_path=`pwd`

#vcpkg path
if [[ $vcpkg_path == "" ]];then
    #find vcpkg in root dir, get the first one
    vcpkg_path_arr=($(find ~ -name "vcpkg" -maxdepth 5))
    if (( ${#vcpkg_path_arr[@]} == 0 ));then
        echo "can't found vcpkg, run vcpkg_path=xxx ./bootstrap.sh"
        exit -1
    fi
    vcpkg_path=${vcpkg_path_arr[0]}
fi

#cmake path
cmake_path=`which cmake`
if [[ $cmake_path == "" ]];then
    echo "can't found cmake, run cmake_path = xxx ./bootstrap.sh"
    exit -1
fi

echo "export sheep_pkg_path=\"$sheep_pkg_path\"" > init_tools/env.sh
echo "export vcpkg_path=\"$vcpkg_path\"" >> init_tools/env.sh
echo "export cmake_path=\"$cmake_path\"" >> init_tools/env.sh
echo "export build_type=\"DEBUG\"" >> init_tools/env.sh
#echo "build_type=\"RELEASE\"" >> init_tools/env.sh
echo "export load_env=\"true\"" >> init_tools/env.sh

cd init_tools
./build_all.sh $1
cd -
