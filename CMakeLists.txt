cmake_minimum_required(VERSION 2.8)

option(ENABLE_UPX "安装后对Screen做upx处理。"  OFF)
option(ENABLE_STRIP "strip处理。"  OFF)
option(ENABLE_BACKTRACE "启用backtrace。"  ON)
option(ENABLE_TEST "编译测试代码。" OFF)
option(ENABLE_MEMCHECK "打开内存泄露检测。" OFF)
option(ENABLE_COVER_TEST  "启用覆盖率测试。" OFF)
option(ENABLE_DISTCC   "使用distcc。" OFF)
option(ENABLE_CCACHE   "使用ccache。" OFF)
option(ENABLE_PLATFORM_OPTIMIZATION   "平台优化，目前只有T2x有用。" ON)
option(ENABLE_PCH "使用预编译头文件" ON)
option(ENABLE_PROF "启用动态分析工具" OFF)
option(ENABLE_NOEXCEPTION "禁用C++异常机制" ON)
option(ENABLE_HIDEVISIABLE "隐藏导出函数" ON)
option(ENABLE_HIDEINLINEVISIABLE "隐藏C++内联导出函数" ON)
option(HOST_REQUIRED "是否要求指定host" ON)
option(ENABLE_JSON, "是否加入Json库" OFF)
option(ENABLE_TESTABILITY "自动化测试开关。" ON)
option(ENABLE_SECTIONSOPTIMIZATION "是否启用sections优化" ON)
option(ENABLE_STRIPCOMMENT "是否启用strip commit优化" ON)
option(ENABLE_DIAGNOSE "是否设备管理平台" ON)
option(ENABLE_C++0X "是否开启stdc++ 0x" ON)
option(ENABLE_DEBUG_LOG "是否开启Debug日志" OFF)

# 编译目标开关
option(BUILD_EXPCOLOR "是否编译expcolor程序" OFF)
option(ENABLE_IDE_MODE "是否IDE编辑模式" ON)
option(BUILD_EXPXWIN "是否XWIN绘制exp" ON)

include(cmakeinclude/cmakedefines.cmake)

# 是否隐藏导出函数
if(T49 OR T48 OR T46 OR T69 OR T42S)
    set(ENABLE_HIDEVISIABLE OFF)
else()
    set(ENABLE_HIDEVISIABLE ON)
endif()

project(${host})

include(cmakeinclude/defines.cmake) # 需要在project之后包含，否则其中部分变量会被覆盖
include(cmakeinclude/fastconfig.cmake)
include(support/support-special.cmake)
include(cmakeinclude/support-features.cmake)
include(cmakeinclude/precompile.cmake)
include(cmakeinclude/post-build.cmake)

# 设置安装路径
option(INSTALL_TO_ROOT_DIR "安装到当前目录，而不是编译目录" OFF)
if(INSTALL_TO_ROOT_DIR)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/dist_${host}")
else()
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/output")
endif()
message("Install prefix: ${CMAKE_INSTALL_PREFIX}")

add_subdirectory(src)
