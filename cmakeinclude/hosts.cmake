# 获取机型信息

# 检查host变量是否在指定机型列表中，如果是则开启指定机型宏
macro(CHECK_HOST host_list host_var)
    list(FIND ${host_list} "${host}" _index)
    if (_index GREATER -1)
        set(${host_var} ON)
    endif()
endmacro()

# 各机型宏对应的机型名称
set(T2XD_HOSTS  "t19" "t21" "t2xd")
set(T2X_HOSTS   "t22" "t26" "t28" "t2x")
set(T20_HOSTS   "t20")
set(T23_HOSTS   "t23" "t61" "t66")
set(T27_HOSTS   "t27" "t41" "t42")
set(T27G_HOSTS  "t27g")
set(T40G_HOSTS  "t40g")
set(T42S_HOSTS  "t42s")
set(T46_HOSTS   "t46")
set(T46S_HOSTS  "t46s")
set(T48_HOSTS   "t48")
set(T48S_HOSTS  "t48s")
set(T49_HOSTS   "t49")
set(T54_HOSTS   "t54")
set(T69_HOSTS   "t69")
set(CP_HOSTS    "cp")
set(DECT_HOSTS  "dect")
set(CP920_HOSTS "cp920")
# 所有支持机型名称列表
set(SUPPORT_HOSTS
    ${T2XD_HOSTS}
    ${T2X_HOSTS}
    ${T20_HOSTS}
    ${T23_HOSTS}
    ${T27_HOSTS}
    ${T27G_HOSTS}
    ${T40G_HOSTS}
    ${T42S_HOSTS}
    ${T46_HOSTS}
    ${T46S_HOSTS}
    ${T48_HOSTS}
    ${T48S_HOSTS}
    ${T49_HOSTS}
    ${T54_HOSTS}
    ${T69_HOSTS}
    ${CP_HOSTS}
    ${DECT_HOSTS}
    ${CP920_HOSTS}
    )

# 校验host变量是否有效
string(TOLOWER "${host}" host)
if(host)
    list(FIND SUPPORT_HOSTS ${host} _index)
    if (_index EQUAL -1)
        message(FATAL_ERROR "Host '${host}' not supported! Supported hosts: ${SUPPORT_HOSTS}")
    endif()
elseif(HOST_REQUIRED)
        message(FATAL_ERROR "Please specify host: -Dhost=${SUPPORT_HOSTS}")
endif()

# 确定机型宏
CHECK_HOST(T2XD_HOSTS   T2XD)
CHECK_HOST(T2X_HOSTS    T2X)
CHECK_HOST(T20_HOSTS    T20)
CHECK_HOST(T23_HOSTS    T23)
CHECK_HOST(T27_HOSTS    T27)
CHECK_HOST(T27G_HOSTS   T27G)
CHECK_HOST(T40G_HOSTS   T40G)
CHECK_HOST(T42S_HOSTS   T42S)
CHECK_HOST(T46_HOSTS    T46)
CHECK_HOST(T46S_HOSTS   T46S)
CHECK_HOST(T48_HOSTS    T48)
CHECK_HOST(T48S_HOSTS   T48S)
CHECK_HOST(T49_HOSTS    T49)
CHECK_HOST(T54_HOSTS    T54)
CHECK_HOST(T69_HOSTS    T69)
CHECK_HOST(CP_HOSTS     CP)
CHECK_HOST(DECT_HOSTS   DECT)
CHECK_HOST(CP920_HOSTS  CP920)

set(XWIN ON)

if(T27G)
    set(T27 ON)
endif()
if(T40G)
    set(T23 ON)
endif()
if(T42S)
    set(T27 ON)
    set(T27G ON)
endif()
if(T46S)
    set(T46 ON)
endif()
if(T48S)
    set(T48 ON)
endif()
if(T54)
    set(T46 ON)
endif()
if(T46 OR T48 OR T69 OR T49)
    set(T4X ON)
endif()
if(T48 OR T49)
    set(XWIN OFF)
endif()
