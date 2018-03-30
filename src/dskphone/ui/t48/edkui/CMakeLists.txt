#################################
#example qt's CMakeLists.txt
#module:edkui
#path:/lib/libedkui.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../../../logic
		../uicommon
		../include
)
qt_build(edkui "so")
