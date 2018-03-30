#################################
#example qt's CMakeLists.txt
#module:testmode
#path:/lib/libtestmode.so
#################################

#使用qt_build添加动态库目标
include_directories(
	../../../logic
	../uicommon
	../include
    include
)
qt_build(testmode "so")
