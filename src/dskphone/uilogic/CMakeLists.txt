if(T49 OR XWIN)

include_directories(
    ../logic
)

collection_build_begin()

if(COLOR)
add_subdirectory(idleuilogic)
add_subdirectory(diruilogic)
add_subdirectory(settinguilogic)
add_subdirectory(talkuilogic)
add_subdirectory(uilogiccommon)
add_subdirectory(viewmodeuimanager)
add_subdirectory(screensaversuilogic)
else()
add_subdirectory(idleuilogic)
add_subdirectory(diruilogic)
add_subdirectory(settinguilogic)
add_subdirectory(talkuilogic)
add_subdirectory(uilogiccommon)
add_subdirectory(viewmodeuimanager)
add_subdirectory(screensaversuilogic)
add_fake_directory(EXCLUDE idleuilogic)
add_fake_directory(EXCLUDE diruilogic)
add_fake_directory(EXCLUDE idleuisettilogic)
add_fake_directory(EXCLUDE talkuilogic)
add_fake_directory(EXCLUDE uilogiccommon)
add_fake_directory(EXCLUDE viewmodeuimanager)
add_fake_directory(EXCLUDE screensaversuilogic)
endif()

if(COLOR AND (NOT MSVC))
	set(LOGIC_LIB_TYPE SHARED)
else()
    set(LOGIC_LIB_TYPE STATIC)
endif()
add_collection_library(${LOGIC_LIB_TYPE})

_WC_GET_BUILD_DIRECTORY_VARS()
#安装动态库
if("${LOGIC_LIB_TYPE}" STREQUAL STATIC)
    target_link_libraries(uilogic
        logic
    )
else()
install(TARGETS ${target_name} LIBRARY DESTINATION lib
                        RUNTIME DESTINATION bin)

endif()

else()
    add_fake_directory()
endif()
