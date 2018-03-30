#################################
#example qt's CMakeLists.txt
#module:Metaswithc UI
#path:/lib/libMetaswitchUI.so
#################################

#使用qt_build添加动态库目标
qt_build(MetaswitchUI "so")
include_directories(
		../../../logic
		../baseui
		../uicommon
		../../
		../include
		../../../../include
		../
)
