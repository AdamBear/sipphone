#################################
#example qt's CMakeLists.txt
#module: customtheme
#path:/lib/libcustomtheme.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../
		../../include
)
qt_build(customtheme "so")
