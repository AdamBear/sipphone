
include_directories(
		../../../../uilogic
		../../../../logic
		../../../uibase
		../../../
		../../
		../../../include
    ../include
)

aux_source_directory(. SRC_FILES)

add_library(talkui OBJECT
    ${SRC_FILES}
    )

