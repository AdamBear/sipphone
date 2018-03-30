#################################
#UI构建通用宏
#用法：
# project_name为目标名
# 默认情况下可执行文件最终生成的名称和目标名一致
# 库在linux下会加上lib前缀和对应.a或.so的后缀
# qt_build会查找当前目录下的源代码和Qt头文件、Qt UI文件和Qt资源文件名进行对应处理，加入到project_name目标中
# qt_build(dskPhone.exx, "exe") 
#
# 可以像一般的目标一样，使用目标名，例如添加依赖
# target_link_libraries(dskPhone.exx
#   ETL
#   pthread
#   ...
#   )
#
#################################

macro(qt_build project_name build_type)
    #由于如果使用本地编译，moc生成的cpp文件会被aux_source_directory包含，造成重复编译
    #所以禁止使用本地编译
    if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
        message("CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
        message("CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
        message(FATAL_ERROR "No support in-place compiling while using qt_build().")
    endif()

    find_package(Qt4 REQUIRED)
	if(ENABLE_OPENGL)
            set(QT_USE_QTOPENGL TRUE)
	endif()
    include(${QT_USE_FILE})
    add_definitions(${QT_DEFINITIONS} -DIS_QT_BUILD=1)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    ##enable auto moc
    set(CMAKE_AUTOMOC ON)
    
    if (MSVC)
        #保存显示名
        set(target_name ${project_name})
        _WC_GET_BUILD_DIRECTORY_VARS()
        _WC_SET_TARGET_DISPLAY_NAME(NAME ${project_name})
    else()
        ##获取源码文件
        _WC_GET_BUILD_DIRECTORY_VARS()
        set(target_name ${project_name})
    endif()

    ##处理Qt头文件（moc）
    #set(${project_name}_HEADERS ...)
    #file(GLOB ${project_name}_HEADERS *.h)
    QT4_WRAP_CPP(${project_name}_HEADERS_MOC ${${project_name}_HEADERS})

    ##处理Qt UI文件（uic）
    #set(${project_name}_FORMS ...)
    file(GLOB ${project_name}_FORMS *.ui src/*.ui)
    QT4_WRAP_UI(${project_name}_FORMS_HEADERS ${${project_name}_FORMS})

    ##处理Qt资源文件(rcc)
    #set(${project_name}_RESOURCES ...)
    file(GLOB ${project_name}_RESOURCES *.qrc)
    QT4_ADD_RESOURCES(${project_name}_RESOURCES_RCC ${${project_name}_RESOURCES})

    list(APPEND SOURCE_FILES 
        ${${project_name}_HEADERS_MOC} 
        ${${project_name}_RESOURCES_RCC})
    list(APPEND HEADER_FILES ${${project_name}_FORMS_HEADERS})
    
	if(MSVC)
        _WC_FILTER_MSVC_PCH(NAME_WE qtheaders)

		if("${build_type}" STREQUAL "exe")
			add_executable(${target_name} ${HEADER_FILES} ${SOURCE_FILES})
			deep_strip(${target_name})
			target_link_libraries(${target_name} ${QT_LIBRARIES})
			#target_install(${target_name})
		else()
			add_library(${target_name} STATIC ${HEADER_FILES} ${SOURCE_FILES})
		endif()
		
        _WC_RESTORE_DISPLAY_NAME()
		_WC_ADD_MSVC_PCH()
	else()
		if("${build_type}" STREQUAL "exe")
			add_executable(${target_name} ${HEADER_FILES} ${SOURCE_FILES})
			deep_strip(${target_name})
		elseif("${build_type}" STREQUAL "a")
			add_library(${target_name} STATIC ${HEADER_FILES} ${SOURCE_FILES})
		elseif("${build_type}" STREQUAL "so")
			add_library(${target_name} SHARED ${HEADER_FILES} ${SOURCE_FILES})
			deep_strip(${target_name})
		endif()
		
		target_link_libraries(${target_name} ${QT_LIBRARIES})
		target_install(${target_name})
	endif()
	
	set_target_folder()
	#set_target_properties(${project_name} PROPERTIES VERSION 1 SOVERSION 1)
endmacro()