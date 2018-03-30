#################################
#example qt's CMakeLists.txt
#module:WiFiUI
#path:/lib/libWiFiUI.so
#################################

#使用qt_build添加动态库目标
include_directories(
	../../
	../../../logic
	../uicommon
	../include
    include
)
qt_build(WiFiUI "so")
