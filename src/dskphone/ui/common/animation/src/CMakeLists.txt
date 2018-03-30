#################################
#module:libanimation.a
#path:null
#################################

aux_source_directory(. SRC_FILES)

include_directories(
	../include 
)

add_library(animation OBJECT
    ${SRC_FILES}
    )
