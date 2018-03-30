#################################
#module:libxWindow.a
#path:null
#################################

add_definitions(-DXWINDOW_EXPORTS)

if(COLOR)
if (MSVC)
add_definitions(-DXMD_H -DHAVE_BOOLEAN)
endif()

configure_file(xgraphicdefine.color.h
               xgraphicdefine.h
               COPYONLY)
configure_file(xscreendefine.color.h
               xscreendefine.h
               COPYONLY)
configure_file(xwindowdefine_sys.color.h
               xwindowdefine_sys.h
               COPYONLY)
configure_file(xwindowdefine_ctrl.color.h
               xwindowdefine_ctrl.h
               COPYONLY)
configure_file(xlistviewdefine.color.h
               xlistviewdefine.h
               COPYONLY)
configure_file(xtableviewdefine.color.h
               xtableviewdefine.h
               COPYONLY)
else()
configure_file(xgraphicdefine.black.h
               xgraphicdefine.h
               COPYONLY)
configure_file(xscreendefine.black.h
               xscreendefine.h
               COPYONLY)
configure_file(xwindowdefine_sys.black.h
               xwindowdefine_sys.h
               COPYONLY)
configure_file(xwindowdefine_ctrl.black.h
               xwindowdefine_ctrl.h
               COPYONLY)
configure_file(xlistviewdefine.black.h
               xlistviewdefine.h
               COPYONLY)
configure_file(xtableviewdefine.black.h
               xtableviewdefine.h
               COPYONLY)
endif()

collect()
