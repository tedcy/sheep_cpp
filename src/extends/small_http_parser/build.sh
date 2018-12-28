set -e

#test if build.sh runned
path=`find . -name libsmall_http_parser.a`
if [[ $path != "" ]];then
    exit 0
fi

path=""
path=`find . -name http-parser`
if [[ $path != "" ]];then
    exit 0
fi
git clone https://github.com/nodejs/http-parser
