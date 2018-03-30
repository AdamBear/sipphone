#include "textmessageviewdelegate.h"
#include "settinguilogic/src/textmessagecontroller.h"
#include "settinguilogic/include/common.h"
#include "settinguitextmessageitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "translateiddefine.h"
#include "settingui/include/pageiddefine.h"

#include "languagehelper/modlangtranslate.h"
#include "baseui/include/modbaseui.h"
#include "settinguimanager.h"
#include "settinguilogic/src/textmessagecontroller.h"
#include "uicallbackfun.h"
#include "feature/include/modfeature.h"

namespace
{
#define MSGBOX_ID_DELETE        1
#define NOTE_MSGBOX_SHOW_TIME   -1
}

CTextMessageViewDelegate::CTextMessageViewDelegate()
    :m_bReply(false),
     m_nTextMember(0)
    
{

}

CTextMessageViewDelegate::~CTextMessageViewDelegate()
{

}

CSettingUIDelegateBase * CTextMessageViewDelegate::CreatTextMessageViewDelegate()
{
    CTextMessageViewDelegate * pDelegate = new CTextMessageViewDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CTextMessageViewDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (NULL == g_pTextMessageController)
    {
        return;
    }

    vecSoftKey[0].m_iType = ST_BACK;
    vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);

    vecSoftKey[2].m_iType = ST_DELETE;
    vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_DELETE);

    TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
    if (TEXT_MSG_GROUP_IN == stDetailData.m_eGType)
    {
        vecSoftKey[1].m_iType = ST_REPLY;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_REPLAY);
    }
    else if (TEXT_MSG_GROUP_SENT == stDetailData.m_eGType
             || TEXT_MSG_GROUP_SENDING == stDetailData.m_eGType)
    {
        vecSoftKey[1].m_iType = ST_FORWARD;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_FORWARD);
    }
}

bool CTextMessageViewDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_REPLY == iType || ST_FORWARD == iType)
    {
        //若是进入in信箱，那么进入界面title显示为to:
        //如果是进入send和out的信箱，那么进入界面显示title应为newmessage
        EnterPageNewMessage();
        return true;
    }
    else if (ST_DELETE == iType)
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE),
                                               _LANG(TRID_IF_DELETE_THIS_MESSAGE)
                                               , SETTING_MESSAGEBOX_OkCancel, "", "", NOTE_MSGBOX_SHOW_TIME
                                               , MSGBOX_ID_DELETE);
        return true;
    }
    else if (ST_BACK == iType)
    {
        if (NULL == g_pTextMessageController)
        {
            return false;
        }

        g_pTextMessageController->ResetTempTextMsgDetailData();
        g_SetUICallBackFun.FallBackToPrevousPage();
        // 由于查看消息界面是只绑定一个delegate的单独窗口，需要主动刷新回退的界面
        g_SetUICallBackFun.RefreshUI(true);
        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CTextMessageViewDelegate::InitExtraData(void * pData)
{
    if (NULL == g_pTextMessageController)
    {
        return;
    }

    TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
    GetShowText(stDetailData.m_strText, m_vecText);
}

bool CTextMessageViewDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /*= NULL*/)
{
    if (NULL == g_pTextMessageController || SETTING_MessageBox_OK != nResult)
    {
        return false;
    }

    if (MSGBOX_ID_DELETE == nID)
    {
        int nCurIndex = g_pTextMessageController->GetTempDataIndex();
        TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
        int nCurGType = stDetailData.m_eGType;
        g_pTextMessageController->RemoveMessage(nCurGType, nCurIndex);

        nCurIndex--;
        if (nCurIndex < 0)
        {
            nCurIndex = 0;
        }
        TextMsgSimpListData txtMsgList;
        if (!g_pTextMessageController->GetMessageList(&txtMsgList, nCurGType)
                || 0 == txtMsgList.m_listMessage.size())
        {
            g_pTextMessageController->ResetTempTextMsgDetailData();
            g_SetUICallBackFun.FallBackToPrevousPage();
        }
        else
        {
            g_pTextMessageController->GetMessageDetail(&stDetailData, nCurGType, nCurIndex);
            g_pTextMessageController->SetTempTextMsgDetailData(stDetailData);
            g_pTextMessageController->SetTempDataIndex(nCurIndex);
            GetShowText(stDetailData.m_strText, m_vecText);
        }
        g_SetUICallBackFun.RefreshUI(true);
    }
    return true;
}

bool CTextMessageViewDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (PHONE_KEY_OK == nKeyCode)
    {
        //如果是进入send和out的信箱，那么进入界面显示title应为newmessage
        EnterPageNewMessage();
        return true;
    }
   
    return false;
}

void CTextMessageViewDelegate::SavePreValue(TextMsgItemDetailData & stDetailData,int nTextMember)
{
    if (m_bReply)
    {
        if (TEXT_MSG_TEXT == nTextMember)
        {
            stDetailData.m_strText = m_strText;
        }
        else if (TEXT_MSG_FROM_USER_NAME == nTextMember)
        {
            stDetailData.m_strFromUserName = m_strFromUserName;
        }
        g_pTextMessageController->SetTempTextMsgDetailData(stDetailData);
        m_bReply = false;
    }
    else
    {
        if (TEXT_MSG_TEXT == nTextMember)
        {
            m_strText = stDetailData.m_strText;
        }
        else if (TEXT_MSG_FROM_USER_NAME == nTextMember)
        {
            m_strFromUserName = stDetailData.m_strFromUserName;
        }
        m_bReply = true;
    }

}

void CTextMessageViewDelegate::EnterPageNewMessage()
{
    TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
    //如果是进入send和out的信箱，那么进入界面显示title应为newmessage
    if (TEXT_MSG_GROUP_SENT == stDetailData.m_eGType ||
        TEXT_MSG_GROUP_SENDING == stDetailData.m_eGType)
    {
        stDetailData.m_strToUserName = "";
        g_pTextMessageController->SetTempTextMsgDetailData(stDetailData);
        m_nTextMember = TEXT_MSG_TEXT;
        SavePreValue(stDetailData, m_nTextMember);
    }
    //若是进入in信箱，那么进入界面title显示为to:
    else if (TEXT_MSG_GROUP_IN == stDetailData.m_eGType)
    {
        m_nTextMember = TEXT_MSG_FROM_USER_NAME;
        SavePreValue(stDetailData, m_nTextMember);
    }
    PageOpenData opPageData(kszPageIDTextMsgNew);
    g_SetUICallBackFun.EnterPage(opPageData);
}
