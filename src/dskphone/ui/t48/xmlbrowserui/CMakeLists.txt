#################################
#example qt's CMakeLists.txt
#module:xmlbrowserui
#path:/lib/libxmlbrowserui.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../../../logic
		../uicommon
		../include
)
qt_build(xmlbrowserui "so")
