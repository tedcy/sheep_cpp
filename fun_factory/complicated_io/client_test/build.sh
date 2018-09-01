set -e
mkdir -pv build 
cd build
cmake .. -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_TOOLCHAIN_FILE="/mnt/sda3/root/vcpkg/scripts/buildsystems/vcpkg.cmake"
make
cd -

