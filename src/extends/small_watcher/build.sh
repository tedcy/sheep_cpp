source "env.sh"
set -e
mkdir -pv build
cd build
build_type="DEBUG"
#build_type="RELEASE"
$cmake_path/cmake .. -DCMAKE_BUILD_TYPE=$build_type -DCMAKE_TOOLCHAIN_FILE=$vcpkg_path -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make -j8
cd -
