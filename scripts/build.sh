if [ "$ " ]
then

    CMAKE_PARM="${CI_BUILD_TAG#*(}"
    CMAKE_PARM="${CMAKE_PARM%)*}"
    PACK_VER="${CI_BUILD_TAG%(*}"

    # trim both white space
    CMAKE_PARM="$(echo -e "${CMAKE_PARM}" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')"
    PACK_VER="$(echo -e "${PACK_VER}" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')"
    CMAKE_PARM="$(echo $CMAKE_PARM | tr ',' ' ')"

    # 存在自定义的cmake参数
    if [ "$CMAKE_PARM" == "$PACK_VER" ]
    then
    CMAKE_PARM=""
    echo "没有检测到自定义的cmake参数，使用默认编译方式"
    else
    CI_BUILD_TAG=$PACK_VER
    echo "检测到自定义的cmake参数 [$CMAKE_PARM]"
    echo "版本号为 [$CI_BUILD_TAG]"
    fi

    DOT_COUNTS=$(echo $CI_BUILD_TAG | tr -cd '.' | wc -c)
    if [ $DOT_COUNTS == "3" ]
    then
        # 获取tag指定的机型
        VER_PREFIX="${CI_BUILD_TAG%%.*}"
        # 大小写不敏感比较
        ANY_DEVICE="x"
        ANY_DEVICE_TMP=$(echo $ANY_DEVICE | tr   [a-z]   [A-Z])
        VER_PREFIX_TMP=$(echo $VER_PREFIX | tr   [a-z]   [A-Z])
        CI_BUILD_NAME_TMP=`echo $CI_BUILD_NAME | sed -r "s/t4[0-9]s/t4xs/"`
		
		CI_BUILD_NAME_TMP=$(echo $CI_BUILD_NAME_TMP | tr   [a-z]   [A-Z])

        if [ "$VER_PREFIX_TMP" != "$ANY_DEVICE_TMP" -a "$VER_PREFIX_TMP" != "$CI_BUILD_NAME_TMP" ]
        then
            echo "tag指定了机型为【$VER_PREFIX】，当前机型为$CI_BUILD_NAME，不执行编译"
            exit 0
        fi
    fi
fi

git submodule sync && git submodule update --init --recursive
LOGIC_CPUS=`cat /proc/cpuinfo | grep "processor" | wc -l`
mkdir make.$CI_BUILD_NAME
cd make.$CI_BUILD_NAME
cmake .. -Dhost=$CI_BUILD_NAME -DINSTALL_TO_ROOT_DIR=ON $CMAKE_PARM
make -j$LOGIC_CPUS install
