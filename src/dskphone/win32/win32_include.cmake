set(SOURCE_FILES)

macro(path_include_for_ide target_path)
	if (MSVC)
        set(HEADER_EXP *.h *.hxx *.hpp *.ixx)
        set(SOURCE_EXP *.c *.cc *.cxx *.cpp *.cxx *.rc)

        foreach(SOURCE_PATH ${HEADER_EXP} ${SOURCE_EXP})
            file(GLOB_RECURSE DIR_HEADER_FILES ${target_path}/${SOURCE_PATH})
            list(APPEND SOURCE_FILES ${DIR_HEADER_FILES})
        endforeach()
        
        foreach(filename ${SOURCE_FILES})
            set_source_files_properties(${filename} PROPERTIES HEADER_FILE_ONLY TRUE)
            get_filename_component(dirname ${filename} PATH)
            file(RELATIVE_PATH dirname ${CMAKE_SOURCE_DIR}/src/dskphone ${dirname})
            string(REPLACE "/" "\\" dirname "${dirname}")
            string(REPLACE ".." "" dirname "${dirname}")
            source_group("${dirname}" FILES ${filename})
        endforeach()
	endif()
endmacro()

# 需要额外引入的源文件路径
path_include_for_ide(${CMAKE_SOURCE_DIR}/src/dskphone/win32/wininc)
path_include_for_ide(${CMAKE_SOURCE_DIR}/src/dskphone/win32/baselibs/include)
path_include_for_ide(${CMAKE_SOURCE_DIR}/src/dskphone/win32/3rdlib/include)
path_include_for_ide(${CMAKE_SOURCE_DIR}/src/baselibs/include)
path_include_for_ide(${CMAKE_SOURCE_DIR}/src/3rdlib/include)
path_include_for_ide(${CMAKE_SOURCE_DIR}/src/include)

# 设置一个custom target来承载这些文件
set(target_name win32_include)

add_custom_target(${target_name}
                COMMAND echo Build a fake target for win32 platform succeed
                COMMENT "Build include target for ide will do nothing but waste of time"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                SOURCES ${SOURCE_FILES})

set_target_properties(${target_name} PROPERTIES 
    OUTPUT_NAME "include"
    PROJECT_LABEL "ALL_INCLUDE"
    FOLDER "CMakePredefinedTargets")
