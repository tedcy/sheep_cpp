set -e
if [[ $load_env == "" ]];then
    if [[ $envResult == "" ]];then
        envResult=($(find . -maxdepth 1 -name env.sh))
        if ((${#envResult[@]} == 0));then
            echo "can't found env.sh"
            exit -1
        fi
        source "env.sh"
    fi
fi
path=`find . -maxdepth 1 -name build.sh`
if [[ $path != "" ]];then
    ./build.sh
    exit 0
fi
mkdir -pv build
cd build
$cmake_path .. -DCMAKE_BUILD_TYPE=$build_type \
    -DCMAKE_TOOLCHAIN_FILE=$vcpkg_path/scripts/buildsystems/vcpkg.cmake \
    -DSHEEP_PROJECT_SOURCE_DIR=$sheep_pkg_path \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make -j8
cd -
