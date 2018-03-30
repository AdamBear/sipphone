#defines.cmake
#flags about cmake,compiler and linker

# 生成VS工程时采用文件夹结构
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

if (MSVC)
    #windows 编译参数
    if (LATTICE)
        add_definitions(-D_WINDOWS)
    else()
        add_definitions(-D_CONSOLE)
    endif()
    #xp 0x0501 win7 0x0601
    add_definitions(-D_WIN32_WINNT=0x0601 -DWINVER=0x0601 -D_WIN32_IE=0x0601)

    #导出库函数
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    #允许install作为默认编译
    set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD ON)

    #msvc兼容性问题
    if (MSVC_VERSION GREATER 1400 OR MSVC_VERSION EQUAL 1400)
        # hashmap兼容性问题
        add_definitions(-D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS)
        # windows.h 头文件包含顺序问题
        add_definitions(-DWIN32_LEAN_AND_MEAN)
    endif()
    if (MSVC_VERSION GREATER 1900 OR MSVC_VERSION EQUAL 1900)
        #指定默认编码
        add_definitions(/utf-8)
        #最小重新生成
        add_definitions(/Gm)
        #启用字符串池
        #add_definitions(/GF)
        #启用最新版本c++特性 c++11 or 14
        add_definitions(/std:c++latest)
        #启用安全检查
        add_definitions(/GS)
    endif()
    if (MSVC_VERSION GREATER 1600)
        #vs编译问题
        add_definitions(-D_XKEYCHECK_H)
        #多线程编译  由于vs默认开启了多工程编译，去掉多线程支持
        #add_definitions(/MP)    
    else()
        #多线程编译
        add_definitions(/MP)    
    endif()

    #工程文件中使用相对路径
    set(CMAKE_SUPPRESS_REGENERATION 1)
    set(CMAKE_USE_RELATIVE_PATHS ON)

    set(CMAKE_SKIP_RPATH OFF)
    set(CMAKE_SKIP_BUILD_RPATH OFF)
    set(CMAKE_SKIP_INSTALL_RPATH OFF)
    #用的不是expree版本
    set(CMAKE_USING_VC_FREE_TOOLS OFF)
    
    #设置默认的pdb、lib文件生成路径，以区分install目录
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_PDB_OUTPUT_DIRECTORY     ${CMAKE_BINARY_DIR}/pdb)

    message("#############################################################################################")
    message( STATUS "CMAKE_VERSION                  : ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
    message( STATUS "CMAKE_SYSTEM                   : ${CMAKE_SYSTEM}")
    message( STATUS "CMAKE_SYSTEM_NAME              : ${CMAKE_SYSTEM_NAME}")
    message( STATUS "CMAKE_SYSTEM_VERSION           : ${CMAKE_SYSTEM_VERSION}")
    message( STATUS "CMAKE_SYSTEM_PROCESSOR         : ${CMAKE_SYSTEM_PROCESSOR}")
    message( STATUS "CMAKE_CL_64                    : ${CMAKE_CL_64}")
    message( STATUS "CMAKE_C_COMPILER               : ${CMAKE_C_COMPILER}")
    message( STATUS "CMAKE_CXX_COMPILER             : ${CMAKE_CXX_COMPILER}")
    message( STATUS "CMAKE_C_FLAGS                  : ${CMAKE_C_FLAGS}")
    message( STATUS "CMAKE_CXX_FLAGS                : ${CMAKE_CXX_FLAGS}")
    message( STATUS "CMAKE_BUILD_TOOL               : ${CMAKE_BUILD_TOOL}")
    message( STATUS "CMAKE_COMMAND                  : ${CMAKE_COMMAND}")
    message( STATUS "CMAKE_GENERATOR                : ${CMAKE_GENERATOR}")
    message( STATUS "CMAKE_ROOT                     : ${CMAKE_ROOT}")
    message( STATUS "CMAKE_BUILD_TYPE               : ${CMAKE_BUILD_TYPE}")
    message( STATUS "CMAKE_SOURCE_DIR               : ${CMAKE_SOURCE_DIR}")
    message( STATUS "CMAKE_BINARY_DIR               : ${CMAKE_BINARY_DIR}")
    message( STATUS "CMAKE_CACHEFILE_DIR            : ${CMAKE_CACHEFILE_DIR}")
    message( STATUS "CMAKE_SIZEOF_VOID_P            : ${CMAKE_SIZEOF_VOID_P}")
    message( STATUS "CMAKE_VS_PLATFORM_TOOLSET      : ${CMAKE_VS_PLATFORM_TOOLSET}")
    message( STATUS "MSVC_IDE                       : ${MSVC_IDE}")
    message( STATUS "MSVC_VERSION                   : ${MSVC_VERSION}")
    message( STATUS "PLATFORM                       : ${PLATFORM}")
    message( STATUS "HOST_TYPE                      : ${host}")
    message("#############################################################################################")
else()
    #cmake defines
    #set default build type to MinSizeRel
    #use Debug mode while test

    if(ENABLE_TEST)
        set(CMAKE_BUILD_TYPE "Debug")
    endif()
    if((NOT CMAKE_BUILD_TYPE) OR "${CMAKE_BUILD_TYPE}" MATCHES "^ *$")
        set(CMAKE_BUILD_TYPE "MinSizeRel")
    endif()

    #DEBUGMODE
    string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
    if(NOT BUILD_TYPE MATCHES "debug|relwithdebuginfo")
        set(DEBUG_MODE OFF)
    else()
        set(DEBUG_MODE ON)
    endif()

    message("Build Type: ${CMAKE_BUILD_TYPE}")

    #compile flags
    #using relative path for __FILE__, save flash space
    set(CMAKE_USE_RELATIVE_PATHS ON)

    #using pipe
    add_definitions(-pipe)

    # using C++11
    if(ENABLE_C++0X)
        add_definitions(--std=c++0x)
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --std=c++0x")
    endif()


    # FIXME: 兼容旧代码，避免编译报错
	if(NOT ENABLE_DISABLEPERMISSIVE)
	    add_definitions(-fpermissive)
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fpermissive")
	endif()

    if(ARMDSPG)
        add_definitions(-Wno-psabi)
    endif()
	
	if(ENABLE_UNWINDTABLES)
		add_definitions(-funwind-tables)
		set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -funwind-tables")
		set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -funwind-tables")
		#UNWIND 开启了，强制关闭BACKSTRACE，关闭STRIP
		set(ENABLE_BACKTRACE OFF)
		set(ENABLE_STRIP OFF)
	endif()

    if(ENABLE_BACKTRACE)
        #关闭去除fp优化
        add_definitions(-fno-omit-frame-pointer)
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fno-omit-frame-pointer")
        set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -fno-omit-frame-pointer")
		#BACKSTRACE 开启了，强制关闭STRIP
		set(ENABLE_STRIP OFF)
    endif()

    #mipsel platform optimization
    if(ENABLE_PLATFORM_OPTIMIZATION)
        if(MIPSEL)
            add_definitions("-mips4")
        elseif(ARM)
            add_definitions("-march=armv5te")
        endif()
    endif()

    #for Qt Path
    if(T46 OR T69 OR T48)
        set(CMAKE_PREFIX_PATH /opt/qt463-all-01)
    elseif(T49)
        if(ENABLE_OPENGL)
            set(CMAKE_PREFIX_PATH /opt/qt486-arago-t49-opengles2)
        else()
            set(CMAKE_PREFIX_PATH /opt/qt486-arago-t49)
        endif()
    endif()

    #Linking flags
    #suppress "-rdynamic" flag
    if(NOT DEBUG_MODE)
        set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
        set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
    endif()

    if(DEBUG_MODE OR ENABLE_DEBUG_LOG)
        add_definitions(-DIF_DEBUG_LOG)
    endif()

    if(ENABLE_PROF)
        add_definitions("-finstrument-functions")
        add_definitions("-finstrument-functions-exclude-file-list=src/3rdlib,src/baselibs,/opt")
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -finstrument-functions")
        set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -finstrument-functions")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -finstrument-functions")
    endif()

    if(ENABLE_HIDEVISIABLE)
        add_definitions("-fvisibility=hidden")
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fvisibility=hidden")
        set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -fvisibility=hidden")
        message ("Info: enable hidden visiable")
    endif()

    if(ENABLE_HIDEINLINEVISIABLE)
        add_definitions("-fvisibility-inlines-hidden")
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fvisibility-inlines-hidden")
        #set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -fvisibility-inlines-hidden")
        message ("Info: enable hidden inline visiable")
    endif()

    if(ENABLE_SECTIONSOPTIMIZATION)
        add_definitions(-ffunction-sections)
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -ffunction-sections")
        set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -ffunction-sections")

        add_definitions(-fdata-sections)
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fdata-sections")
        set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -fdata-sections")

        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-gc-sections")
        set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-gc-sections")
        message ("Info: enable sections optimization")
    endif()

    if(ENABLE_NOEXCEPTION)
        add_definitions(-DIF_NO_EXCEPTION)
        add_definitions("-fno-exceptions")
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fno-exceptions")
        set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -fno-exceptions")
        message ("Info: enable no exception")
    endif()

    if(ENABLE_NORTTI)
        add_definitions(-DIF_NO_RTTI)
        add_definitions("-fno-rtti")
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fno-rtti")
        set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -fno-rtti")
        message ("Info: enable no rtti")
    endif()

    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors")
    
    #warning as error
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=overflow")

    #括号不匹配。
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-braces")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsequence-point")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-macros")
    
    #以下选项打开警告过多
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winline")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")

    #GCC 7 才支持
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wimplicit-fallthrough=3")
    #GCC 4.9才支持
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsizeof-pointer-memaccess")

    #黑白机使用supc++（supc++改名为stdc++，避免外部模块修改makefile）
    if(NOSTD)
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lstdc++")
    endif()

    #runtime path
    set(RUNTIME_BIN_PATH /bin:/boot/bin:/phone/bin)
    set(RUNTIME_LIB_PATH /lib:/boot/lib:/phone/lib:./)
    if(DEBUG_MODE)
        set(RUNTIME_LIB_PATH ./:./lib:/mnt/lib:/mnt:/tmp/lib:${RUNTIME_LIB_PATH})
    endif()

    # the RPATH to be used when installing
    set(CMAKE_INSTALL_RPATH "${RUNTIME_LIB_PATH}")

    message("==========configure info==========" )
    message("Platform: ${PLATFORM}" )
    message("Build tool prefix: ${BUILDTOOL_PREFIX}" )
    message("C compiler: ${CMAKE_C_COMPILER}")
    message("C++ compiler: ${CMAKE_CXX_COMPILER}")
    message("ar: ${CMAKE_AR}")
    message("linker: ${CMAKE_LINKER}")
    message("nm: ${CMAKE_NM}")
    message("objcopy: ${CMAKE_OBJCOPY}")
    message("objdump: ${CMAKE_OBJDUMP}")
    message("ranlib: ${CMAKE_RANLIB}")
    message("strip: ${CMAKE_STRIP}")
    message("compile flag: ${CMAKE_CXX_FLAGS}")
    message("link flag: ${CMAKE_CXX_LINK_FLAGS}")
    message("==========configure info==========" )
endif()
