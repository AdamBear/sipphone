#################################
#example qt's CMakeLists.txt
#module:MessageBox
#path:/lib/libMessageBox.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../
		../../include
)
qt_build(MessageBox "so")
