#################################
#example qt's CMakeLists.txt
#module:talkui
#path:/lib/libtalkui.so
#################################

#使用qt_build添加动态库目标
include_directories(
	../../../logic
	../uicommon
	../include
    include
)
qt_build(talkui "so")
