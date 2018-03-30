add_definitions(-D_DEBUGMEM)
aux_source_directory(. SRC_FILES)

add_library(memory STATIC
	${SRC_FILES}
	)
