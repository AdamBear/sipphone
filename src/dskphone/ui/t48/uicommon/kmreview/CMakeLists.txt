#################################
#example qt's CMakeLists.txt
#module:KmReview 
#path:/lib/libKmReview .so
#################################

#使用qt_build添加动态库目标
qt_build(KmReview "so")
include_directories(
		../../../logic
		../uicommon
		../include
)