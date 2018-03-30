message("Use Json Library" )
aux_source_directory(. SRC_FILES)

add_library(cjson STATIC
	${SRC_FILES}
	)
	