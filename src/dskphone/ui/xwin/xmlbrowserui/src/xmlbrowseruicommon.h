#ifndef _XML_BROWSER_UICOMMON_H
#define _XML_BROWSER_UICOMMON_H

#include "extended/xmlbrowser/include/logicanduicommon.h"
#include "xwindow/xGraphic.h"

int TypeToAlignmentFlag(int eAlignType);

// lineColor转换为颜色
xColor TypeToColor(TEXT_FONT_COLOR eColorType);

int TypeToFontPointSize(X_FONT_TYPE eFontSize);

// 根据softkey的类型转化为softkey
SoftkeyParam TypeToSoftInfo(XB_SOFTKEY_TYPE eType);

// 逻辑层和UI层之间的softkey数据转换
yl::string ToUISoftkeyAction(const SoftkeyParam & strLogicAction);
SoftkeyParam ToLogicSoftkeyAction(const yl::string & strUIAction);

#endif // _XML_BROWSER_UICOMMON_H
