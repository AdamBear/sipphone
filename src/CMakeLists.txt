#header install
function(header_install target name)
	install(FILES ${target} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${name})
endfunction()

#install object
function(target_install target)
	if (MSVC)
		install (TARGETS ${target} 
			RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
			ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
			LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
        # pdb 文件安装路径
        install (FILES $<TARGET_PDB_FILE:${target}> 
			DESTINATION ${CMAKE_INSTALL_PREFIX}/pdb OPTIONAL)
	else(MSVC)
	    install (TARGETS ${target} 
        	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
	endif(MSVC)
endfunction()

#win32 common link property
function(add_target_link_property target)
    if(MSVC)
        # vs 链接参数
        if (MSVC_VERSION GREATER 1500)
            set_target_properties(${target} PROPERTIES LINK_FLAGS "/VERBOSE:Lib /MANIFEST:NO")
        else()
            set_target_properties(${target} PROPERTIES LINK_FLAGS "/VERBOSE:Lib")
        endif()
        
        # win32 app (entry point WinMain OR main)
        if (LATTICE OR XWIN)
            set_target_properties(${target} PROPERTIES WIN32_EXECUTABLE TRUE)
        else()
            set_target_properties(${target} PROPERTIES WIN32_EXECUTABLE FALSE)
        endif()
		
		#set_target_properties(${target} PROPERTIES
        #    VS_SCC_PROJECTNAME "gui"
        #    VS_SCC_LOCALPATH ${CMAKE_SOURCE_DIR}
        #    VS_SCC_PROVIDER "Yealink")
        
        # 添加工作目录
        set_target_properties(${target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX})
        
        if(${host_dir} STREQUAL "${PHONE_HOST_NAME}")
            message("set target ${target} as vs startup project")
            # 设置默认的启动工程
            set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${target})
        endif()
    endif()
endfunction()

# 资源路径定义
force_include_header_file(path_define.h)
# 屏蔽平台差异性
force_include_header_file(platform.h)

if (MSVC)
    set(LINK_LIB_BASELIBS 	    
            TaskAction
            ETL
            ETLMsgHandler
            CommonAPI
            DeviceLib
            xmlparser
            ConfigParser
            regexp
            resource
            YLSTL
            Translate
        )
        
    set(LINK_LIB_3RD
            winimp
            libdl
            libservice
            libmsgkit
            libsysservice
            libautopservice
            pthreadVSE2
            libcfgservice
            libeay32
            ssleay32
        )
        
	if(COLOR)
		set(LINK_LIB_3RD ${LINK_LIB_3RD} freetype libjpeg libpng16)
	endif()
	
	if(ENABLE_JSON)
		set(LINK_LIB_3RD ${LINK_LIB_3RD} cjson)
	endif()

    # win32 平台根目录
    set(WIN32_ROOT_PATH ${CMAKE_SOURCE_DIR}/src/dskphone/win32)
    _WC_SET_FOLDER_ROOT_DIR()
    
	include_directories(
        ${WIN32_ROOT_PATH}
        ${WIN32_ROOT_PATH}/wininc 
        ${WIN32_ROOT_PATH}/baselibs
        ${WIN32_ROOT_PATH}/baselibs/include 
        ${WIN32_ROOT_PATH}/baselibs/include/msgkit
        ${WIN32_ROOT_PATH}/3rdlib
        ${WIN32_ROOT_PATH}/3rdlib/include
    )

	# 预编译头文件目录
	if (ENABLE_PCH)	
        set(PCH_PATH ${WIN32_ROOT_PATH})
	set(PCH_HEADER_PATH ${PCH_PATH}/precompile.h)
	set(PCH_SOURCE_PATH ${PCH_PATH}/precompile.cpp)
	endif()
    
    # build type > platform version > platform
    link_directories(${WIN32_ROOT_PATH}/3rdlib/lib/${PLATFORM}/${CMAKE_VS_PLATFORM_TOOLSET}/${CMAKE_BUILD_TYPE})
    link_directories(${WIN32_ROOT_PATH}/baselibs/lib/${CFG_BACKEND}/${PLATFORM}/${CMAKE_VS_PLATFORM_TOOLSET}/${CMAKE_BUILD_TYPE})
    link_directories(${WIN32_ROOT_PATH}/3rdlib/lib/${PLATFORM}/${CMAKE_VS_PLATFORM_TOOLSET})
    link_directories(${WIN32_ROOT_PATH}/baselibs/lib/${CFG_BACKEND}/${PLATFORM}/${CMAKE_VS_PLATFORM_TOOLSET})
    link_directories(${WIN32_ROOT_PATH}/3rdlib/lib/${PLATFORM})
    link_directories(${WIN32_ROOT_PATH}/baselibs/lib/${CFG_BACKEND}/${PLATFORM})
endif()

include_directories(
	.
	include
	include/commondefine
	include/configiddefine
	baselibs/include
	baselibs/include/ETLLib
	baselibs/include/ylstl
	baselibs/include/xmlparser
	baselibs/include/configparser
	3rdlib/include
	)
	
if(COLOR)
	include_directories(3rdlib/include/freetype2)
endif()

if(DECT)
    include_directories(
        3rdlib/include/dect/libdect/include
        )
endif()

if(ENABLE_TEST)
    include_directories(
        3rdlib/googletest/googlemock/include
        3rdlib/googletest/googletest/include
        )
endif()

#library set
link_directories(${CMAKE_SOURCE_DIR}/src/3rdlib/lib/${PLATFORM})
#baselib
link_directories(${CMAKE_SOURCE_DIR}/src/baselibs/lib/${CFG_BACKEND}/${PLATFORM})

if(ENABLE_TEST)
    _WC_SET_FOLDER_ROOT_DIR()
endif()

add_all_subdirectories()
