#################################
#example qt's CMakeLists.txt
#module:idleui
#path:/lib/libidleui.so
#################################

#使用qt_build添加动态库目标
include_directories(
	include 
	../../../logic
	../uicommon
	../include
)
qt_build(dsskeyui "so")
