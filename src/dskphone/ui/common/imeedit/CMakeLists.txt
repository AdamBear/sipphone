#################################
#module:libimeedit.a
#path:null
#################################

add_definitions(-DXWINDOW_EXPORTS)

if(COLOR)
configure_file(imeeditdefine.color.h
               imeeditdefine.h
               COPYONLY)
else()
configure_file(imeeditdefine.black.h
               imeeditdefine.h
               COPYONLY)
endif()

collect()
