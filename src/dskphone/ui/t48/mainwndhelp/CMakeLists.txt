#################################
#example qt's CMakeLists.txt
#module:MainWndHelp
#path:/lib/libMainWndHelp.so
#################################

#使用qt_build添加动态库目标
qt_build(MainWndHelp "so")
include_directories(
		../../../logic
		../uicommon
		../include
)