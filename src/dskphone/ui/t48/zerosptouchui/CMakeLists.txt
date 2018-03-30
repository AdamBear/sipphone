#################################
#example qt's CMakeLists.txt
#module:zerosptouchui
#path:/lib/libzerosptouchui.so
#################################


include_directories(
	include 
	../../../logic
	../uicommon
	../include
)
qt_build(zerosptouchui "so")
