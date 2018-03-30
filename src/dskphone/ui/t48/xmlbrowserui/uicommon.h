#ifndef UICOMMON_H
#define UICOMMON_H
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include <QtGui>

// lineColor转换为颜色
QColor TypeToColor(TEXT_FONT_COLOR eColorType);

Qt::AlignmentFlag TypeToAlignmentFlag(int eAlignType);

int TypeToFontPointSize(X_FONT_TYPE eFontSize);

// 根据softkey的类型转化为softkey
SoftkeyParam TypeToSoftInfo(XB_SOFTKEY_TYPE eType);

//获取softkey图标
QString SoftkeyTypeToPicPath(XB_SOFTKEY_TYPE eType);

#endif // UICOMMON_H
