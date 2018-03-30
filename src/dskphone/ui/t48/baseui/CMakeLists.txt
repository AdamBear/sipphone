#################################
#example qt's CMakeLists.txt
#module:BaseUI
#path:/lib/libBaseUI.so
#################################

#使用qt_build添加动态库目标
qt_build(BaseUI "so")
include_directories(
		../uicommon
		../include
)