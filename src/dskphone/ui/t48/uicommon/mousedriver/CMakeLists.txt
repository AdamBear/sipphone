#################################
#example qt's CMakeLists.txt
#module:MouseDriver
#path:/lib/libylMouseDriverPlugin.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../
		../../include
)
qt_build(ylMouseDriverPlugin "so")

if (NOT MSVC)
    install(TARGETS ylMouseDriverPlugin 
        LIBRARY DESTINATION lib/plugins/mousedrivers
        RUNTIME DESTINATION bin
    )
endif()
