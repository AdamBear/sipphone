if(_USB AND _USB_AUDIO)

collect()

include_directories(
    include 
    src
)

add_mock_library(STATIC)

endif()
