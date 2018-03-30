build_host()
{
    LOGIC_CPUS=`cat /proc/cpuinfo | grep "processor" | wc -l`
    #编译
    mkdir make.$1
    cd make.$1
    cmake .. -Dhost=$1 -DINSTALL_TO_ROOT_DIR=ON >xxx.log 2>&1 
    make -j$LOGIC_CPUS install >xxx.log 2>&1
    DISTPATH="dist_$1"

    #计算大小
    cd ..
	python ./scripts/caclfilesize.py "${DISTPATH}"  
}

build_host "t23"
build_host "t42s"
build_host "t48s"
build_host "t54"
build_host "cp920"