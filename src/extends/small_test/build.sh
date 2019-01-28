set -e

#test if build.sh runned
path=`find build -maxdepth 1 -name libgmock.a`
if [[ $path != "" ]];then
    exit 0
fi

path=""
path=`find . -name googletest-master`
if [[ $path != "" ]];then
    exit 0
fi
wget https://codeload.github.com/google/googletest/zip/master
mv master gtest.zip
unzip gtest.zip
rm -rf gtest.zip
cd googletest-master
mkdir -pv build
cd build
cmake ..
make
cd ../..
mkdir -pv build
cp googletest-master/build/lib/libgtest.a build/
cp googletest-master/build/lib/libgmock.a build/
cp -r googletest-master/googletest/include/gtest/ .
cp -r googletest-master/googlemock/include/gmock/ .
