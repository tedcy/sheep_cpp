set -e
clean=$1
export envResult=($(find . -maxdepth 1 -name env.sh))
if ((${#envResult[@]} == 0));then
    echo "can't found env.sh"
    exit -1
fi
source "env.sh"

small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/log")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_pprof")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_hiredis")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_packages")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_config")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_net")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_timer")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_http_client")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/extends/small_watcher")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/small_server")
small_pkgs_arr=(${small_pkgs_arr[@]} "$sheep_pkg_path/src/net")

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
        $sheep_pkg_path/init_tools/build.sh
        echo "----------$v building end------------------"
        echo "-------------------------------------------"
        echo "-------------------------------------------"
        cd $v
        echo "----------$v/test building start-----------"
        cd $v/test
        $sheep_pkg_path/init_tools/build.sh
        echo "----------$v/test building end-------------"
        echo "-------------------------------------------"
        echo "-------------------------------------------"
        cd $v
    done
fi
