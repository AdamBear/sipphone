#ifndef __MOD_BASE_UI_H__
#define __MOD_BASE_UI_H__

#include "messagebox/messageboxcallback.h"
#include "baseuicommon.h"

void BaseUI_Init();
void BaseUI_UnInit();

/***********************************************************/
// 功能: 弹出菜单项选择框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//                  listoption: 传入的菜单项内容
//                  iID: 弹出框ID
/***********************************************************/
void BaseUI_ShowOptionMenu(CMsgBoxCallBackAction * pAction, VEC_OPTION_ITEM_DATA & vecOption,
                           int nID = -1);
#if IS_COLOR
void BaseUI_ImageInit();
void BaseUI_GetNormalDialogBG(xBrush& bursh);
#endif

yl::string BaseUI_GetRealShowText(const yl::string& strText, bool bShowIndex, int nIndex);

#endif
