#ifndef __X_WINDOW_DEFINE_H__
#define __X_WINDOW_DEFINE_H__

#include "xwindow-kernel/include/xkerneldefine.h"

#ifdef XWINDOW_EXPORTS
#define XWINDOW_EXPORT  DECLARE_EXPORT
#else
#define XWINDOW_EXPORT  DECLARE_IMPORT
#endif

#ifndef XWINDOW_CONST
#define XWINDOW_CONST DECLARE_CONST
#endif

// 是否使用命名空间
#define IF_XWINDOW_NAMESPACE_ENABLE 1

#if IF_XWINDOW_NAMESPACE_ENABLE
#define BEGIN_XWINDOW_NAMESPACE     namespace NS_xWindow {
#define END_XWINDOW_NAMESPACE       }
#define USING_XWINDOW_NAMESPACE     using namespace NS_xWindow;
#else
#define BEGIN_XWINDOW_NAMESPACE
#define END_XWINDOW_NAMESPACE
#define USING_XWINDOW_NAMESPACE
#endif

////////////////////////////////////////////////////////////////////////////
// 调试信息
////////////////////////////////////////////////////////////////////////////
#define IF_XWINDOW_DEBUG 0
#if IF_XWINDOW_DEBUG
#define IF_DEBUG_VIEW_MGR           0
#define IF_DEBUG_PAINTER            0
#define IF_DEBUG_PARSER             0
#define IF_DEBUG_EVENT              0
#define IF_DEBUG_GRAPHIC_SCALE      0
#define IF_DEBUG_MOUSE_EVENT        0
#define IF_DEBUG_PAINTE_TIME        0
#define IF_DEBUG_VIEW_FOCUS         0
#endif
#define IF_XGRAPHIC_DEBUG 0

#define IF_XWINDOW_DUMP_EXPORT 0
#if IF_XWINDOW_DUMP_EXPORT
#define IF_XWINDOW_AUTO_TEST 0
#endif

// "xgraphicdefine.black.h" "xgraphicdefine.color.h"
#include <xwindow/xgraphicdefine.h>
// "xwindowdefine_sys.black.h" "xwindowdefine_sys.color.h"
#include <xwindow/xwindowdefine_sys.h>
// "xwindowdefine_ctrl.black.h" "xwindowdefine_ctrl.color.h"
#include <xwindow/xwindowdefine_ctrl.h>

#endif
