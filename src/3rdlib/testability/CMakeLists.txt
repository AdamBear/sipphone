aux_source_directory(. SRC_FILES)

add_library(testability STATIC
	${SRC_FILES}
	)
