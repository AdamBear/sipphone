#################################
#example qt's CMakeLists.txt
#module:MainWnd
#path:/lib/libMainWnd.so
#################################

#使用qt_build添加动态库目标
qt_build(MainWnd "so")
include_directories(
		../../../logic
		../uicommon
		../include
)