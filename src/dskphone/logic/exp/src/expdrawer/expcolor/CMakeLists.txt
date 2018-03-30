#################################
#module:expcolor.exx
#path:/bin/expcolor.exx
#################################

if(BUILD_EXPCOLOR)
if(BUILD_EXPXWIN)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    ##enable auto moc
    set(CMAKE_AUTOMOC ON)
    
    #set_target_properties(${project_name} PROPERTIES VERSION 1 SOVERSION 1)
    
    include_directories(
            ../expbase/
            ../../expdriveheader
            ${CMAKE_SOURCE_DIR}/src/dskphone/ui/t46/uicommon
            ${CMAKE_SOURCE_DIR}/src/dskphone/ui/t46/include
	    ${CMAKE_SOURCE_DIR}/src/dskphone/ui/
            ${CMAKE_SOURCE_DIR}/src/include
	    ${CMAKE_SOURCE_DIR}/src/dskphone/ui/common/xwindow
            #../../../ui/t46/uicommon
            #../../../ui/t46/include
            #../../../../baselibs/include
    
    )

    add_fake_target()
    collect()
else()
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

    ##source file
    aux_source_directory(. expcolor.exx_SOURCES)
	aux_source_directory(../expbase expcolor.exx_SOURCES)
    ##处理Qt头文件（moc）
    #set(${project_name}_HEADERS ...)
    #file(GLOB ${project_name}_HEADERS *.h)
    QT4_WRAP_CPP(expcolor.exx_HEADERS_MOC ${${project_name}_HEADERS})

    ##处理Qt UI文件（uic）
    #set(${project_name}_FORMS ...)
    file(GLOB expcolor.exx_FORMS *.ui)
    QT4_WRAP_UI(expcolor.exx_FORMS_HEADERS ${${project_name}_FORMS})

    ##处理Qt资源文件(rcc)
    #set(${project_name}_RESOURCES ...)
    file(GLOB expcolor.exx_RESOURCES *.qrc)
    QT4_ADD_RESOURCES(expcolor.exx_RESOURCES_RCC ${${project_name}_RESOURCES})
	
	file(GLOB other_HEADER ../expbase/*.h)
	file(GLOB other_SOURCE ../expbase/*.cpp)
	
    set(expcolor.exx_ALL_SOURCE 
        ${expcolor.exx_SOURCES} 
        ${expcolor.exx_HEADERS_MOC}
        ${expcolor.exx_FORMS_HEADERS} 
        ${expcolor.exx_RESOURCES_RCC}
		${other_HEADER}
		${other_SOURCE}
    )
	
    add_executable(expcolor.exx ${expcolor.exx_ALL_SOURCE})
	deep_strip(expcolor.exx)
    

    target_link_libraries(expcolor.exx ${QT_LIBRARIES})
    
    #set_target_properties(${project_name} PROPERTIES VERSION 1 SOVERSION 1)
    
    include_directories(
            ../expbase
            ../../expdriveheader
            ${CMAKE_SOURCE_DIR}/src/dskphone/ui/t46/uicommon
            ${CMAKE_SOURCE_DIR}/src/dskphone/ui/t46/include
            #../../../ui/t46/uicommon
            #../../../ui/t46/include
            #../../../../baselib/include
    )

    if(ENABLE_MEMCHECK)
        include_directories(
                ../../../3rdlib/memory
        )
        target_link_libraries(expcolor.exx
           memory
           )
    endif()

    if (MSVC)
        set_target_properties(expcolor.exx
                 PROPERTIES OUTPUT_NAME "expcolor"
            )
        
        set(LIB_NAME_PREFIX "t46_")
            
        target_link_libraries(expcolor.exx
            ${LINK_LIB_BASELIBS}
            ${LIB_NAME_PREFIX}ImageManager
            libmsgkit
            winimp
        )
    else()
        target_link_libraries(expcolor.exx
            ETLMsgHandler
            resource  
            CommonAPI
	    ssl
	    crypt
	    crypto
            xmlparser
            DeviceLib
            ConfigParser
            regexp
        )

        target_link_libraries(expcolor.exx
            ETL
            ImageManager
            sysservice
            YLSTL
        )
    endif()

    deep_strip(expcolor.exx)

    target_install(expcolor.exx)
endif()
	
else()
    add_fake_target()
endif()