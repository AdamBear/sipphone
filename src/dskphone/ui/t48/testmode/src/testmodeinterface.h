/************************************************************************
 * FileName  : modTestMode.h (header file)
 * Purpose   :
 * Date      : 2012/11/13 14:39:36
 ************************************************************************/

#ifndef _TESTMODE_INTERFACE_H_
#define _TESTMODE_INTERFACE_H_

#include "cdlgtestmode.h"

/************************************************************************/
/* 在测试模式对话框上绘制文本                                                */
/************************************************************************/
void testmode_DrawText(QString strText, bool bCenter = true, int fontsize = -1,
                       QColor clrTx = Qt::black, QColor clrBk = Qt::white);
void testmode_DrawTextInRect(QString strText, QRect rectText, QColor clrTx = Qt::black);

/************************************************************************/
/* 在测试模式对话框上填充颜色                                                 */
/************************************************************************/
void testmode_FullScreen(QColor clr);
void testmode_FullScreen(QString strPic);

void testmode_SetFrontColor(QColor clr);
void testmode_SetAllHide();
void testmode_SetFrameAndTitleVisible(TEST_STATE tstate);

void testmode_SetEasyTouch(bool bShow, QPoint point, int nSize);
bool testmode_getBeTouched();

#endif // _TESTMODE_INTERFACE_H_
