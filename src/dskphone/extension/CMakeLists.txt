add_fake_directory()

#编译为动态库
set(EXTENSION_LIB_TYPE SHARED)

set(SRC_FILES 
    service_dsk_interface_extension.cpp
    )
    
add_library(extension OBJECT
    ${SRC_FILES}
    )
    
add_library(dskservice ${EXTENSION_LIB_TYPE}
  	$<TARGET_OBJECTS:extension>
    )

install(TARGETS dskservice LIBRARY DESTINATION lib
                           RUNTIME DESTINATION bin)

