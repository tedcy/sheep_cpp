source "env.sh"
set -e
mkdir -pv build
cd build
#$cmake_path/cmake .. -DCMAKE_BUILD_TYPE="RELEASE" -DCMAKE_TOOLCHAIN_FILE=$vcpkg_path
$cmake_path/cmake .. -DCMAKE_BUILD_TYPE="DEBUG" -DCMAKE_TOOLCHAIN_FILE=$vcpkg_path
make -j8
cd -


mkdir -pv build/log
