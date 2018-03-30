#################################
#example qt's CMakeLists.txt
#module:globalInputHook
#path:/lib/libglobalInputHook.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../
		../../include
)
qt_build(globalInputHook "so")
