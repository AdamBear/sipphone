/************************************************************************
 * FileName  : modTestMode.h (header file)
 * Purpose   :
 * Date      : 2012/11/13 14:39:36
 ************************************************************************/

#ifndef _TESTMODE_INTERFACE_H_
#define _TESTMODE_INTERFACE_H_

#include "test_include.h"

/************************************************************************/
/* 在测试模式对话框上绘制文本                                                */
/************************************************************************/
void testmode_DrawText(yl::string strText, int nTextType = TT_CONTENT_CENTER,
                       int nFontSize = TEST_FONT_20);

void testmode_SetTextColor(int nTextType, const xColor & col);

/************************************************************************/
/* 在测试模式对话框上填充颜色                                                 */
/************************************************************************/
void testmode_FillRect(int nAreaType, const xColor & col);

void testmode_FillRect(const yl::string& strPic);

#ifdef _CP920
void testmode_ShowErrorKeys(bool bShow);
#endif

/************************************************************************/
/* 在测试模式对话框上填充颜色                                                 */
/************************************************************************/
CTestPhone * testmode_GetCurrentTestPhone();

CTestPhone* testmode_CreateTestPhone();

#endif // _TESTMODE_INTERFACE_H_
