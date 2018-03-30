#ifndef _TALKUI_MSGBOX_MANAGER_H
#define _TALKUI_MSGBOX_MANAGER_H

#include <ylstl/ylstring.h>
#include "messagebox/messageboxmanager.h"
#include "talkuiframemanager.h"

namespace talkui
{

//弹框的类型  与子窗口有关  与talkuiframe有关
enum TALKUI_MSGBOX_TYPE
{
    TALKUI_MSGBOX_TYPE_NONE = 0,
    TALKUI_MSGBOX_TYPE_TALKUI,                  //与子窗口有关  切换子窗口的时候就应该移除  t48和t49 机型不包括dial
    TALKUI_MSGBOX_TYPE_TALKUIFRAME,             //与talkuiframe有关  切换子窗口的时候不应该移除

};

class CTalkUIMsgboxManager: public CMsgBoxCallBackAction
{
public:
    static CTalkUIMsgboxManager & GetInstance();

protected:
    CTalkUIMsgboxManager();

public:
    virtual ~CTalkUIMsgboxManager();

public:
    //弹出通话错误提示框
    void ShowTalkUINoteMsgBox(yl::string strErrorInfo
                              , TALKUI_MSGBOX_TYPE eTalkUIMsgboxType = TALKUI_MSGBOX_TYPE_TALKUI
                              , int nTime = TALK_MSGBOX_SHORT_TIME);

public:
    //弹出框模块回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    //移除弹出框
    void RemoveMessageBox(CMessageBoxBase * pMessageBox,
                          TALKUI_MSGBOX_TYPE eTalkUIMsgboxType = TALKUI_MSGBOX_TYPE_TALKUI);

protected:
    //弹框类型
    TALKUI_MSGBOX_TYPE m_eTalkUIMsgboxType;

};

#define g_objTalkUIMsgboxManager CTalkUIMsgboxManager::GetInstance()

}       //namespace talkui

#endif
