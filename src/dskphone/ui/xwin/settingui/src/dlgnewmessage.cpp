#include "dlgnewmessage.h"

#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"
#include "settingui/src/uicallbackfun.h"
#include "settingui/src/uicommonfun.h"
#include "settingui/include/modsettingui.h"
#include "settinguilogic/src/textmessagecontroller.h"
#include "devicelib/networkserver.h"

#include "imeedit/ximeedit.h"
#include "xwindow/xWindowManager.h"
#include "baseui/include/inputmethod.h"
#include "baseui/include/baseuicommon.h"
#include "baseui/include/titlebase.h"
#include "settingui/include/pageiddefine.h"
#include "uicallbackfun.h"
#include "edk/include/modedk.h"
#include "edkui/include/modedkui.h"
#include "feature/include/modfeature.h"
#include "settinguiadapter.h"

namespace
{
#define  NEW_MESSAGE_XML            "setting/dlgnewmessage.xml"
#define  NEW_MESSAGE_EDIT           "editMessage"
}

CBaseDialog * CDlgNewMessage::CreateInstance()
{
    CDlgNewMessage * pDlg = new CDlgNewMessage;

    if (NULL != pDlg)
    {
        pDlg->loadContent(NEW_MESSAGE_XML);
    }

    return pDlg;
}

CDlgNewMessage::CDlgNewMessage()
    : m_pEdit(NULL)
{

}

CDlgNewMessage::~CDlgNewMessage()
{

}

void CDlgNewMessage::Init()
{
    if (NULL == g_pTextMessageController)
    {
        return;
    }

    TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
    if (NULL != m_pEdit && TEXT_MSG_GROUP_IN != stDetailData.m_eGType)
    {
        m_pEdit->SetInitText(stDetailData.m_strText);
        if (feature_UserModeConfigItemReadOnly(kszPageIDTextMsgNew))
        {
            m_pEdit->SetEnabled(false);
        }
        else
        {
            m_pEdit->SetEnabled(true);
        }
    }

    RefreshTitle();
}

void CDlgNewMessage::Uninit()
{

}

void CDlgNewMessage::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }
}

void CDlgNewMessage::SetCommonSoftkey()
{
    CArraySoftKeyBarData vecSoftKey;

    CSoftKeyBarData objSoftkeyData;

    objSoftkeyData.m_iType = ST_BACK;
    objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_BACK);
    vecSoftKey.push_back(objSoftkeyData);

    objSoftkeyData.m_iType = ST_EMPTY;
    objSoftkeyData.m_strSoftkeyTitle = "";
    vecSoftKey.push_back(objSoftkeyData);

    objSoftkeyData.m_iType = ST_EMPTY;
    objSoftkeyData.m_strSoftkeyTitle = "";
    vecSoftKey.push_back(objSoftkeyData);

    if (feature_UserModeConfigItemReadOnly(kszPageIDTextMsgNew))
    {
        objSoftkeyData.m_iType = ST_EMPTY;
        objSoftkeyData.m_strSoftkeyTitle = "";
    }
    else
    {
        objSoftkeyData.m_iType = ST_SEND;
        objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_SEND);
    }
    vecSoftKey.push_back(objSoftkeyData);

    SetSoftKey(vecSoftKey);
}

bool CDlgNewMessage::OnSoftkeyAction(int iType, bool bLongPress)
{
    if (bLongPress)
    {
        return false;
    }
    if (ST_SEND == iType && NULL != m_pEdit)
    {
        if (NULL == g_pTextMessageController)
        {
            return false;
        }
        yl::string strSend = m_pEdit->GetText();
        if (strSend.empty())
        {
            //默认消息的类型为note，设置框内容，弹出弹框
            CSettingUIDataValidInfo dataValid;
            dataValid.SetValidNote(_LANG(TRID_TEXT_CANNOT_BE_EMPTY));
            g_SetUICallBackFun.DealWithValidData(dataValid);
        }
        else
        {
            //进入发送消息的界面
            TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
            stDetailData.m_strText = strSend;
            g_pTextMessageController->SetTempTextMsgDetailData(stDetailData);
            PageOpenData opPageData(kszPageIDTextMsgSend);
            g_SetUICallBackFun.EnterPage(opPageData);
        }
        return true;
    }
    else if (ST_BACK == iType || ST_CANCEL == iType)
    {
        OnExitAction();
        g_SetUICallBackFun.FallBackToPrevousPage();
        // 由于新建消息界面是只绑定一个delegate的单独窗口，需要主动刷新回退的界面
        g_SetUICallBackFun.RefreshUI(true);
        TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
        //若是新信息，则返回需要清除信息的所有数据
        if (TEXT_MSG_GROUP_TYPE_NONE == stDetailData.m_eGType)
        {
            g_pTextMessageController->ResetTempTextMsgDetailData();
        }
        return true;
    }

    return CDlgSettingUI::OnSoftkeyAction(iType, bLongPress);
}

void CDlgNewMessage::loadChildrenElements(xml_node & node)
{
    CDlgSettingUI::loadChildrenElements(node);
    m_pEdit = static_cast<xEditTextView *>(getViewById(NEW_MESSAGE_EDIT));
}

void CDlgNewMessage::OnReturnToIdle()
{
    OnExitAction();
    CDlgSettingUI::OnReturnToIdle();
}

void CDlgNewMessage::RefreshTitle()
{
    TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
    yl::string strTitle;
    if (TEXT_MSG_GROUP_IN == stDetailData.m_eGType)
    {
        strTitle = _LANG(TRID_TO);
        strTitle += ":";
        strTitle += stDetailData.m_strFromUserName;
#ifndef IF_LCD_SIZE_132_64
        if (28 == devicelib_GetPhoneType()
                && !stDetailData.m_strFromServer.empty())
        {
            strTitle += "@";
            strTitle += stDetailData.m_strFromServer;
        }
#endif  // IF_LCD_SIZE_132_64
    }
    else if (!stDetailData.m_strToUserName.empty())
    {
        // 该信息若为newmessage进来或者为draft，则title不显示为newmessage
        strTitle = _LANG(TRID_TO);
        strTitle += ":";
        strTitle += stDetailData.m_strToUserName;
        // 在draft信箱title显示为带有服务器的账号.
        if (!stDetailData.m_strToServer.empty() && TEXT_MSG_GROUP_DRAFT == stDetailData.m_eGType)
        {
            strTitle += "@";
            strTitle += stDetailData.m_strToServer;
        }
    }
    else
    {
        strTitle = _LANG(TRID_NEW_MESSAGE);
    }

    RefreshTitle(strTitle);
}

void CDlgNewMessage::RefreshTitle(const yl::string & strTitle)
{
    if (NULL == m_pTitle)
    {
        return ;
    }

    yl::string strRealtitle = strTitle;
    if (Edk_IsEnable() && EDK_IsTipModeEnable())
    {
        strRealtitle = EdkUI_GetMenuIDByPageData(kszPageIDTextMsgNew, "", -1);
    }

    m_pTitle->SetTitle(strRealtitle);
}

void CDlgNewMessage::OnExitAction()
{
    if (NULL == m_pEdit || NULL == g_pTextMessageController)
    {
        return;
    }

    yl::string strSend = m_pEdit->GetText();
    if (strSend.empty())
    {
        return;
    }

    TextMsgSendItemData stMsgSendData;
    stMsgSendData.m_strText = strSend;
    TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
    stMsgSendData.m_strTo = stDetailData.m_strToUserName;
    stMsgSendData.m_tTime = netGetLocalTime();
    if (!stDetailData.m_strToServer.empty())
    {
        stMsgSendData.m_strTo += ("@" + stDetailData.m_strToServer);
    }

    // 如果信息是草稿信息，则后退时是修改信息,其他都是增加一条草稿信息
    if (TEXT_MSG_GROUP_DRAFT == stDetailData.m_eGType)
    {
        // 加入草稿前，先删除当前的信息
        g_pTextMessageController->RemoveMessage(TEXT_MSG_GROUP_DRAFT,
                                                g_pTextMessageController->GetTempDataIndex());
        // 保存草稿箱后，清除缓存信息
        g_pTextMessageController->ResetTempTextMsgDetailData();
    }

    g_pTextMessageController->AddDraftMessage(stMsgSendData);
}

bool CDlgNewMessage::onKeyPressedEvent(int key)
{
    if (NULL == g_pTextMessageController)
    {
        return false;
    }
    //dlgnewmessage没有delegate，返回键处理
    int nFocusDataIndex = (NULL != m_pListView) ? m_pListView->GetFocusItemDataIndex() : -1;
    if (PHONE_KEY_CANCEL == key && OnProcessAction(ST_BACK, false, nFocusDataIndex))
    {
        TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
        //在onexitaction存储草稿信息
        OnExitAction();
        //若是新信息，则返回需要清除信息的所有数据
        if (TEXT_MSG_GROUP_TYPE_NONE == stDetailData.m_eGType)
        {
            g_pTextMessageController->ResetTempTextMsgDetailData();
        }
        //处理完了就不再继续处理了
        return true;
    }
    if (PHONE_KEY_OK == key)
    {
        yl::string strSend = m_pEdit->GetText();
        //判断信息是否为空
        if (strSend.empty())
        {
            //默认消息的类型为note，设置框内容，弹出弹框
            CSettingUIDataValidInfo dataValid;
            dataValid.SetValidNote(_LANG(TRID_TEXT_CANNOT_BE_EMPTY));
            g_SetUICallBackFun.DealWithValidData(dataValid);
        }
        else
        {
            //不为空则进入发送界面
            TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
            stDetailData.m_strText = strSend;
            g_pTextMessageController->SetTempTextMsgDetailData(stDetailData);
            PageOpenData opPageData(kszPageIDTextMsgSend);
            g_SetUICallBackFun.EnterPage(opPageData);
        }
        //处理完毕
        return true;
    }

    //其余走基类处理
    return CDlgSettingUI::onKeyPressedEvent(key);
}

void CDlgNewMessage::RefreshUI(bool bReLoadData)
{
    CDlgSettingUI::RefreshUI(bReLoadData);
    RefreshTitle();
    SetCommonSoftkey();
}
