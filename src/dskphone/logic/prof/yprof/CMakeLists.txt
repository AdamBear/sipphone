#yprof

project(yprof)
cmake_minimum_required(VERSION 2.8)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS program_options filesystem system date_time)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

aux_source_directory(. SRC_FILES)

add_definitions(-std=c++11)

add_executable(yprof
    ${SRC_FILES}
    )
    
target_link_libraries(yprof
    ${Boost_LIBRARIES}
    )