#################################
#example qt's CMakeLists.txt
#module:InputMethod
#path:/lib/libInputMethod.so
#################################

#使用qt_build添加动态库目标
include_directories(
		../
		../../include
)
qt_build(InputMethod "so")
