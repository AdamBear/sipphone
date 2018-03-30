#################################
#example qt's CMakeLists.txt
#module:idleui
#path:/lib/libidleui.so
#################################

include_directories(
	include
	src
	  ../../../logic
	  ../../../uilogic
		../../common
		../../include
		../../uibase
		../include
		../
)

collect()