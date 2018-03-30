#################################
#example qt's CMakeLists.txt
#module:idleui
#path:/lib/libidleui.so
#################################

#使用qt_build添加动态库目标
qt_build(ScreenSavers "so")
include_directories(
	../../../logic
	../uicommon
	../include
	include
)
