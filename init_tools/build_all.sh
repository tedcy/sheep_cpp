set -e
clean=$1
export envResult=($(find . -maxdepth 1 -name env.sh))
if ((${#envResult[@]} == 0));then
    echo "can't found env.sh"
    exit -1
fi
source "env.sh"

small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/log")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_http_parser")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_pprof")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_hiredis")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_packages")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_config")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_net")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_timer")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_http_client")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/net")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/small_client")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_watcher")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/small_server")

if [[ $clean == "clean" ]]; then
    for v in ${small_pkgs_arr[@]};do
        cd $v
        $sheep_pkg_path/init_tools/clean.sh
        cd $v/test
        $sheep_pkg_path/init_tools/clean.sh
    done
    else
    for v in ${small_pkgs_arr[@]};do
        echo "----------$v building start----------------"
        cd $v
        export buildshFound=($(find . -maxdepth 1 -name build.sh))
        if ((${#buildshFound[@]} != 0));then
            ./build.sh
        fi
        export cmakeFound=($(find . -maxdepth 1 -name CMakeLists.txt))
        if ((${#cmakeFound[@]} != 0));then
            $sheep_pkg_path/init_tools/build.sh
        fi
        echo "----------$v building end------------------"
        echo "-------------------------------------------"
        echo "-------------------------------------------"
        echo "----------$v/test building start-----------"
        cd $v/test
        $sheep_pkg_path/init_tools/build.sh
        echo "----------$v/test building end-------------"
        echo "-------------------------------------------"
        echo "-------------------------------------------"
        cd $v
        export found=($(find build -maxdepth 1 -name compile_commands.json -mmin 1))
        if ((${#found[@]} == 0));then
            cp test/build/compile_commands.json build
        fi
    done
fi
