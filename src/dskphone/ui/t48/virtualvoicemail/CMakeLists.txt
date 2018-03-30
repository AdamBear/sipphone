#################################
#example qt's CMakeLists.txt
#module:Virtual Voice Mail
#path:/lib/libVirtualVoiceMail.so
#################################

#使用qt_build添加动态库目标
qt_build(VirtualVoiceMail "so")
include_directories(
		../../../logic
		../baseui
		../uicommon
		../../
		../include
		../../../../include
		../
)
