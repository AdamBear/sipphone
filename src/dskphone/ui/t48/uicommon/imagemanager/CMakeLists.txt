#################################
#example qt's CMakeLists.txt
#module:ImageManager
#path:/lib/libImageManager.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../
		../../include
)
qt_build(ImageManager "so")
