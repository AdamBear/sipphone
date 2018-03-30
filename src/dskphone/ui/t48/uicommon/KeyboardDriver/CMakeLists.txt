#################################
#example qt's CMakeLists.txt
#module:KeyboardDriver
#path:/lib/ylKeyboardDriverPlugin.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../
		../../include
)
qt_build(ylKeyboardDriverPlugin "so")

install(TARGETS ylKeyboardDriverPlugin 
    LIBRARY DESTINATION lib/plugins/kbddrivers
)