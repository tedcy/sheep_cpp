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
mkdir -pv build
cd build
$cmake_path .. -DCMAKE_BUILD_TYPE=$build_type \
    -DCMAKE_TOOLCHAIN_FILE=$vcpkg_path/scripts/buildsystems/vcpkg.cmake \
    -DSHEEP_PROJECT_SOURCE_DIR=$sheep_pkg_path \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if [[ `which compdb` != "" ]];then
    complieCommandsJsonResult=($(find . -maxdepth 1 -name compile_commands.json))
    if ((${#complieCommandsJsonResult[@]} != 0));then
        compdb -p . list > tmp_compile_commands.json
        mv tmp_compile_commands.json compile_commands.json
    fi
fi
make -j8
cd -
