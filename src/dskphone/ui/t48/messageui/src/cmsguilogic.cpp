#include "cmsguilogic.h"
#include "cdlgmessageframe.h"
#include "cdlgmessageedit.h"
#include "cdlgmessageview.h"

#include "setting/src/textmessagecontroller.h"
#include "setting/src/dateandtimecontroller.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/messageboxbase.h"
#include "uimanager/moduimanager.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "account/include/modaccount.h"
#include <QtCore/QStringList>

#include "settingui/include/modsettingui.h"

// 每页显示的Item数
const int PER_PAGE_COUNT = 7;

CMsgUILogic * CMsgUILogic::s_pInstance = NULL;

CMsgUILogic * CMsgUILogic::GetInstance()
{
    if (NULL == s_pInstance)
    {
        s_pInstance = new CMsgUILogic();
    }

    return s_pInstance;
}

void CMsgUILogic::ReleaseInstance()
{
    if (s_pInstance != NULL)
    {
        delete s_pInstance;
        s_pInstance = NULL;
    }
}

CMsgUILogic::CMsgUILogic(void)
    : m_iMsgListPageIdx(0)
    , m_strMsgBoxAction("")
    , m_bOnePage(false)
{
}

CMsgUILogic::~CMsgUILogic(void)
{
}

//加载第page页的短信列表
bool CMsgUILogic::LoadMsgList(int page)
{
    if (m_strMsgBoxAction.empty())
    {
        return false;
    }

    // 获取页面指针
    CDlgMessageFrame * pSubMenu = (CDlgMessageFrame *)(UIManager_GetPrivateDlg(DLG_CDlgMessageFrame));

    if (NULL == pSubMenu
            || page < 0)
    {
        return false;
    }

    m_iMsgListPageIdx = page;

    TextMsgSimpListData sMsgList;
    //设置本次要获取的个数
    sMsgList.m_nIndexFrom = m_iMsgListPageIdx * PER_PAGE_COUNT;
    sMsgList.m_nCountToGet = PER_PAGE_COUNT;
    if (g_pTextMessageController->GetMessageList(&sMsgList, m_strMsgBoxAction))
    {
        pSubMenu->SetMessageList(sMsgList);
        return true;
    }
    else
    {
        return false;
    }
}

bool CMsgUILogic::GetMsgList(int iPage, TextMsgSimpListData * sMsgList)
{
    if (m_strMsgBoxAction.empty())
    {
        return false;
    }

    m_iMsgListPageIdx = iPage;

    //设置本次要获取的个数
    sMsgList->m_nIndexFrom = m_iMsgListPageIdx * PER_PAGE_COUNT;
    sMsgList->m_nCountToGet = PER_PAGE_COUNT;
    return g_pTextMessageController->GetMessageList(sMsgList, m_strMsgBoxAction);
}

//更新短信列表
bool CMsgUILogic::UpdateMsgList()
{
    while (m_iMsgListPageIdx > -1
            && !LoadMsgList(m_iMsgListPageIdx))
    {
        --m_iMsgListPageIdx;
    }

    if (m_iMsgListPageIdx < 0)
    {
        m_iMsgListPageIdx = 0;
        return false;
    }
    else
    {
        return true;
    }
}

void CMsgUILogic::EnterNewMessage()
{
    SubMenuData subData;
    subData.strMenuAction = "OpenSubPage(text_message_edit_message)";
    SettingUI_OpenPage(subData);
}

//进入Read Message界面
void CMsgUILogic::EnterViewMessage(int iMsgIdx)
{
    m_iMsgCurrentId = iMsgIdx;
    SetBoxAction(kszCAInBox);
    m_bOnePage = true;

    SubMenuData subData;
    subData.strMenuAction = "OpenSubPage(text_message_view_message)";
    SettingUI_OpenPage(subData);
}

void CMsgUILogic::EnterMsgListBox(int iMsgIdx)
{
    SetBoxAction(kszCAInBox);

    SubMenuData subData;
    subData.strMenuAction = "OpenSubPage(text_message_inbox)";
    SettingUI_OpenPage(subData);
}

//获取详细信息
bool CMsgUILogic::GetMessageDetail(TextMsgItemDetailData * pData,
                                   const yl::string & strClickAction, int nIndex) const
{
    bool bSucess = g_pTextMessageController->GetMessageDetail(pData, strClickAction, nIndex);
    if (bSucess)
    {
        pData->m_strTime = Timet2DetailString(pData->m_tTime);
    }
    return bSucess;
}

void CMsgUILogic::ExitMessageDetail()
{
    SetBoxAction("");
    SetCurrentMsgId(-1);
}

//删除一条短信
void CMsgUILogic::DeleteMessage(int iMsgIdx)
{
    if (!m_strMsgBoxAction.empty()
            && iMsgIdx > -1)
    {
        g_pTextMessageController->RemoveMessage(m_strMsgBoxAction, iMsgIdx);
        //UpdateMsgList();
        // 获取页面指针
        CDlgMessageFrame * pSubMenu = (CDlgMessageFrame *)(UIManager_GetPrivateDlg(DLG_CDlgMessageFrame));

        if (NULL == pSubMenu)
        {
            return;
        }
#ifdef LOAD_ALL_DATA_FROM_LOGIC
        pSubMenu->SetDelStatus(true);
#endif
    }
}

//清空指定收件箱中的短信
void CMsgUILogic::ClearCurrentMsgBoxMessages()
{
    if (!m_strMsgBoxAction.empty())
    {
        g_pTextMessageController->RemoveAllMessage(m_strMsgBoxAction);
    }
}

//发送短信
bool  CMsgUILogic::DoSendMessage(const QString & strToAddr, int nFromAccountId,
                                 CDlgMessageEdit * pWnd)
{
    if (strToAddr.trimmed().isEmpty())
    {
        return false;
    }

    if (NULL == pWnd)
    {
        return false;
    }

    TextMsgSendItemData messageData;
    messageData.m_nFromAccountId = nFromAccountId;

    messageData.m_strText = pWnd->GetTextContent().toUtf8().data();
    messageData.m_strTo = strToAddr.trimmed().toUtf8().data();
    messageData.m_tTime = g_pDateAndTimeController->GetLocalTime();

    g_pTextMessageController->SendTheMessage(messageData);

    //删除此条草稿
    if (m_strMsgBoxAction == kszCADraftBox
            && m_iMsgCurrentId > -1)
    {
        DeleteMessage(m_iMsgCurrentId);
    }
    //消息发送完成状态。

    pWnd->OnBtnBackClick();

    return true;
}

//保存草稿
void CMsgUILogic::SaveDraftMessage(int iMsgIdx, bool bUpdateCont/* = true*/,
                                   bool bUpdateAddrs/* = true*/, CDlgMessageEdit * pWnd/* = NULL*/)
{
    TextMsgSendItemData sendData;
    TextMsgItemDetailData pageData;

    GetMessageDetail(&pageData, m_strMsgBoxAction, iMsgIdx);
    sendData.m_strTo = pageData.m_strToUserName;

    sendData.m_tTime = g_pDateAndTimeController->GetLocalTime();

    if (kszCAInBox == m_strMsgBoxAction
            || kszCASentBox == m_strMsgBoxAction
            || kszCAOutBox == m_strMsgBoxAction)
    {
        sendData.m_strText = pageData.m_strText;
    }
    else if (kszCADraftBox == m_strMsgBoxAction)
    {
        sendData.m_strText = pageData.m_strText;
        g_pTextMessageController->RemoveMessage(m_strMsgBoxAction, iMsgIdx);
    }
    else
    {
        //
    }

    if (NULL == pWnd)
    {
        return;
    }
    //更新内容信息
    if (bUpdateCont)
    {
        sendData.m_strText = pWnd->GetTextContent().toUtf8().data();
    }

    //更新地址信息
    if (bUpdateAddrs)
    {
        sendData.m_nFromAccountId = pWnd->GetFromAccountID();
        sendData.m_strTo = pWnd->GetToAddr().toUtf8().data();
    }

    if (!sendData.m_strText.empty()
            || !sendData.m_strTo.empty())
    {
        //添加草稿
        g_pTextMessageController->AddDraftMessage(sendData);
    }
}

