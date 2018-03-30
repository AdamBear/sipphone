#libprof.a
#link in target executable

if(ENABLE_PROF)

include_directories(
    yprof
    )

add_fast_library(STATIC)

endif()
