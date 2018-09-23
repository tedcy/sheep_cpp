path=`find . -maxdepth 1 -name clean.sh`
if [[ $path != "" ]];then
    ./clean.sh
    exit 0
fi
rm -rf build
