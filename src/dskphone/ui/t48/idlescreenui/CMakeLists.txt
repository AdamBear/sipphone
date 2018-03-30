#################################
#example qt's CMakeLists.txt
#module:idleui
#path:/lib/libidleui.so
#################################

#使用qt_build添加动态库目标
qt_build(idleui "so")
include_directories(
		../uicommon
		../include
)