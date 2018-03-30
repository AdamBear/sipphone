#################################
#example qt's CMakeLists.txt
#module:UIManager
#path:/lib/libUIManager.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../
		../../include
)
qt_build(UIManager "so")
