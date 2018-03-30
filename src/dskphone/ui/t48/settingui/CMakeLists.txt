#################################
#example qt's CMakeLists.txt
#module:settingui
#path:/lib/libsettingui.so
#################################

#使用qt_build添加动态库目标
include_directories(
	../../../logic
	../uicommon
	../include
    include
    ../virtualvoicemail
    ../
)
qt_build(settingui "so")
