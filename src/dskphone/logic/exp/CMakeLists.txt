#collect()

include_directories(
    include 
    src
)

string(TOLOWER "${host}" host_lower)
# 设置exp和ehs机型支持情况
if(${host_lower} STREQUAL t54)

	set(BUILD_EXPCOLOR ON)
	add_definitions(-D_EXP_COLOR_SUPPORT)
	if(BUILD_EXPXWIN)
	add_definitions(-D_XWIN)
	endif()
	message("Currnt host is ${host}, and BUILD_EXPCOLOR ON")
	message("Currnt host is ${host}, and _XWIN ON")
	
elseif((${host_lower} STREQUAL t46) OR (${host_lower} STREQUAL t46s) OR (${host_lower} STREQUAL t48) OR (${host_lower} STREQUAL t48s) 
		OR (${host_lower} STREQUAL t27) OR (${host_lower} STREQUAL t27g) OR (${host_lower} STREQUAL t66) OR (${host_lower} STREQUAL t42s))
		
	add_definitions(-D_EXP_BLACK_SUPPORT -D_EHS_SUPPORT)
	
else()

	add_definitions(-D_EHS_SUPPORT)
	
endif()

#IF(ARMDSPG)
#ADD_SUBDIRECTORY(srcfake)
#MESSAGE("exp not compiled")
#ELSE()
ADD_SUBDIRECTORY(src)
MESSAGE("exp compiled")
#ENDIF()