#include "talkuimsgboxmanager.h"
#include "dsklog/log.h"
#include "messagebox/modmessagebox.h"
#include "talkuilogic/talkuilogiccommon.h"

namespace talkui
{

// 弹出框note时间
#ifndef SHORT_TIME
#define SHORT_TIME           2000
#endif

CTalkUIMsgboxManager & CTalkUIMsgboxManager::GetInstance()
{
    static CTalkUIMsgboxManager s_objTalkUIMsgboxManager;

    return s_objTalkUIMsgboxManager;

}

CTalkUIMsgboxManager::CTalkUIMsgboxManager()
//弹框类型
    : m_eTalkUIMsgboxType(TALKUI_MSGBOX_TYPE_NONE)
{


}

CTalkUIMsgboxManager::~CTalkUIMsgboxManager()
{

}

//弹出通话错误提示框
void CTalkUIMsgboxManager::ShowTalkUINoteMsgBox(yl::string strErrorInfo
        , TALKUI_MSGBOX_TYPE eTalkUIMsgboxType
        , int nTime/* = TALK_MSGBOX_SHORT_TIME*/)
{
    m_eTalkUIMsgboxType = eTalkUIMsgboxType;

    MSGBOX_INFO stMsgInfo;
    stMsgInfo.pAction = this;
    stMsgInfo.eType = MESSAGEBOX_NOTE;
    stMsgInfo.strTitle = _UILOGIC_LANG_TRAN(TRID_NOTE);
    stMsgInfo.strNote = strErrorInfo;
    stMsgInfo.nMsTime = nTime;

    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

//弹出框模块回调
void CTalkUIMsgboxManager::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    //赋值弹框类型
    m_eTalkUIMsgboxType = TALKUI_MSGBOX_TYPE_NONE;
}

//移除弹出框
void CTalkUIMsgboxManager::RemoveMessageBox(CMessageBoxBase * pMessageBox,
        TALKUI_MSGBOX_TYPE eTalkUIMsgboxType)
{
    TALKUI_INFO("CTalkUIMsgboxManager::RemoveMessageBox MessageBox: 0x%08x", pMessageBox);
    //如果移除为空那么可以直接返回
    if ((TALKUI_MSGBOX_TYPE_NONE == eTalkUIMsgboxType)
            || (TALKUI_MSGBOX_TYPE_NONE == m_eTalkUIMsgboxType))
    {
        return ;
    }

    //相同肯定就直接移除了  不相同才需要判断
    //如果是要移除所有 那么肯定全部都移除了
    //如果保存的类型不同 那么直接返回
    if (m_eTalkUIMsgboxType != eTalkUIMsgboxType)
    {
        //如果需要移除的是 子窗口 那么直接返回  这边可以确定是后面不需要判断了
        if (TALKUI_MSGBOX_TYPE_TALKUI == eTalkUIMsgboxType)
        {
            return ;
        }

        //移除的肯定是talkuiframe了 那么无论是什么类型都要移除
    }

    MessageBox_RemoveMessageBox(this);
}

}       //namespace talkui

