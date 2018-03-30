#################################
#example qt's CMakeLists.txt
#module:QtCommon
#path:/lib/libQtCommon.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../
		../../include
)
qt_build(QtCommon "so")
