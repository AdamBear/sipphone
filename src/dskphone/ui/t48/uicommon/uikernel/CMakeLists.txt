#################################
#example qt's CMakeLists.txt
#module:UIKernel
#path:/lib/libUIKernel.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../
		../../include
)
qt_build(UIKernel "so")
