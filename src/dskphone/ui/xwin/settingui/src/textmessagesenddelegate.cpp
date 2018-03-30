#include "textmessagesenddelegate.h"
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "devicelib/phonedevice.h"
#include "translateiddefine.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/textmessagecontroller.h"
#include "devicelib/networkserver.h"
#include "settingui/include/modsettingui.h"

namespace
{
#define SET_IME_DIAL "dial"
#define SET_DEFAULT_123 "123"
#define ITEM_MAX_LENGTH 99
#define MSGBOX_ID_SEND          3
#define NOTE_MSGBOX_SHOW_TIME   1500
}

CTextMessageSendDelegate::CTextMessageSendDelegate()
{

}

CTextMessageSendDelegate::~CTextMessageSendDelegate()
{

}

CSettingUIDelegateBase * CTextMessageSendDelegate::CreateTextMessageSendDelegate()
{
    CTextMessageSendDelegate * pDelegate = new CTextMessageSendDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CTextMessageSendDelegate::OnMessage(msgObject & msg)
{
    if (ACCOUNT_STATUS_CHANGED == msg.message)
    {
        CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDTextMsgSend);
        if (NULL != pDelegate)
        {
            if (pDelegate == g_pSettingUIManager->GetTopDelegate())
            {
                g_SetUICallBackFun.RefreshUI(true, kszPageIDTextMsgSend);
            }
            else
            {
                pDelegate->SetRefreshDataWhenTopDelegate(true);
            }

            return TRUE;
        }
    }

    return FALSE;
}

#if IF_SUPPORT_SELECT_CONTACT_INFO
void CTextMessageSendDelegate::UpdateContactInfo(const yl::string& strContactName, const yl::string& strContactNumber)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDTextMsgSend);
    if (NULL == pDelegate)
    {
        return;
    }

    CTextMessageSendDelegate* pTextMsgSendDelegate = static_cast<CTextMessageSendDelegate*>(pDelegate);
    if (NULL == pTextMsgSendDelegate)
    {
        return;
    }

    pTextMsgSendDelegate->UpdateSendData(strContactNumber);
}

void CTextMessageSendDelegate::UpdateSendData(const yl::string& strContactNumber)
{
    SetValue(ITEMID_TEXTMESSAGE_SEND_TO, strContactNumber);
}

bool CTextMessageSendDelegate::IsShowDirectorySoftkey()
{
    yl::string strItemID = GetItemIDByDataIndex(GetFocusIndex());
    yl::string strValue = GetValue(strItemID);
    if (ITEMID_TEXTMESSAGE_SEND_TO == strItemID && strValue.empty())
    {
        return true;
    }

    return false;
}
#endif

CSettingUIPageDataBase * CTextMessageSendDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL != m_pPageData && bReLoad)
    {
        LoadTextMessageSendItemData(m_pPageData);
    }

    return m_pPageData;
}

void CTextMessageSendDelegate::LoadTextMessageSendItemData(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    CSettingUIItemDataBase * pItemData = pPageData->GetItemByItemId(ITEMID_TEXTMESSAGE_SEND_FORM);
    CSettingUIComboBoxItemDataPtr pComboboxItem = CSettingUIComboBoxItemData::GetComboBoxDataItem(
                pItemData);
    if (NULL == pComboboxItem)
    {
        return;
    }

    int iAccountNum = acc_AccountNum();
    int nValidAccount = 0;
    for (int i = 0; i < iAccountNum; ++i)
    {
        // 帐号开启才可以发送
        if (acc_IsAccountAvailable(i))
        {
            CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;
            if (NULL == pOption)
            {
                continue;
            }

            pOption->m_strDisplayName = acc_GetAccountShowText(i);;
            pOption->m_strItemVale = commonAPI_FormatString("%d", i);
            if (!pComboboxItem->AddOptionItem(pOption))
            {
                delete pOption;
            }
            ++nValidAccount;
        }
    }

    // 如果账号都不能用,则使用empty
    if (0 == nValidAccount)
    {
        CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;
        if (NULL != pOption)
        {
            pOption->m_strDisplayName = _LANG(TRID_ACCOUNT_EMPTY);;
            pOption->m_strItemVale = commonAPI_FormatString("%d", INVALID_ACCOUNTID);
            if (!pComboboxItem->AddOptionItem(pOption))
            {
                delete pOption;
            }
        }
    }

    if (NULL == g_pTextMessageController)
    {
        return;
    }

    TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
    yl::string strValue;
    if (TEXT_MSG_GROUP_IN == stDetailData.m_eGType)
    {
        strValue = stDetailData.m_strFromUserName;
#ifndef IF_LCD_SIZE_132_64
        if (28 == devicelib_GetPhoneType()
                && !stDetailData.m_strFromServer.empty())
        {
            strValue += ("@" + stDetailData.m_strFromServer);
        }
#endif  // IF_LCD_SIZE_132_64
    }
    else
    {
        strValue = stDetailData.m_strToUserName;
#ifndef IF_LCD_SIZE_132_64
        if (28 == devicelib_GetPhoneType()
                && !stDetailData.m_strToServer.empty())
        {
            strValue += ("@" + stDetailData.m_strToServer);
        }
#endif  // IF_LCD_SIZE_132_64
    }
    SetValue(ITEMID_TEXTMESSAGE_SEND_TO, strValue);
}

void CTextMessageSendDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
#if IF_SUPPORT_SELECT_CONTACT_INFO
    CDirectoryAssociateBaseDelegate::GetPagePrivateSoftkey(vecSoftKey);
#endif

    vecSoftKey[3].m_iType = ST_SAVE;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SEND);
}

bool CTextMessageSendDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (NULL == g_pTextMessageController)
    {
        return false;
    }

    if (ST_SAVE == iType)
    {
        yl::string strSendTo = GetValue(ITEMID_TEXTMESSAGE_SEND_TO);
        int nAccountId = GetComboboxValue(ITEMID_TEXTMESSAGE_SEND_FORM);

        if (INVALID_ACCOUNTID == nAccountId)
        {
            CSettingUIDataValidInfo dataValid;
            int nDataIndex = GetItemDataIndexByItemId(ITEMID_TEXTMESSAGE_SEND_TO);
            dataValid.SetValidIndex(nDataIndex);
            dataValid.SetValidNote(_LANG(TRID_INVALID_ACCOUNT));
            g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        }
        else if (strSendTo.empty())
        {
            CSettingUIDataValidInfo dataValid;
            int nDataIndex = GetItemDataIndexByItemId(ITEMID_TEXTMESSAGE_SEND_TO);
            dataValid.SetValidIndex(nDataIndex);
            dataValid.SetValidNote(_LANG(TRID_TARGET_CANNOT_BE_EMPTY));
            g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        }
        else
        {
            TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
            TextMsgSendItemData stMsgSendData;
            stMsgSendData.m_nFromAccountId = nAccountId;
            stMsgSendData.m_strTo = strSendTo;
            stMsgSendData.m_tTime = netGetLocalTime();
            stMsgSendData.m_strText = stDetailData.m_strText;
            g_pTextMessageController->SendTheMessage(stMsgSendData);
            if (TEXT_MSG_GROUP_DRAFT == stDetailData.m_eGType)
            {
                int nMessageIndex = g_pTextMessageController->GetTempDataIndex();
                // 删除草稿信息
                g_pTextMessageController->RemoveMessage(TEXT_MSG_GROUP_DRAFT, nMessageIndex);
            }
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_SENDING_MESSAGE)
                                                   , SETTING_MESSAGEBOX_NOTE, "", "", NOTE_MSGBOX_SHOW_TIME
                                                   , MSGBOX_ID_SEND);
        }
        return true;
    }
    else if (ST_BACK == iType)
    {
        OnBackAction();
        yl::string strSendTo = GetValue(ITEMID_TEXTMESSAGE_SEND_TO);
        int nAccountId = GetComboboxValue(ITEMID_TEXTMESSAGE_SEND_FORM);
        TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
        if (INVALID_ACCOUNTID != nAccountId)
        {
            stDetailData.m_strToUserName = strSendTo;
        }
        // 如果是收件箱，需修改它的来信方
        if (TEXT_MSG_GROUP_IN == stDetailData.m_eGType)
        {
            stDetailData.m_strFromUserName = strSendTo;
            stDetailData.m_strFromServer = "";
        }
        stDetailData.m_strToUserName = strSendTo;
        g_pTextMessageController->SetTempTextMsgDetailData(stDetailData);
        g_SetUICallBackFun.FallBackToPrevousPage();
        g_SetUICallBackFun.RefreshUI(true);
        return true;
    }
#if IF_SUPPORT_SELECT_CONTACT_INFO
    else if (ST_DIRECTORY == iType)
    {
        SettingUI_EnterPage(kszPageIDDirectory, (void*)&CTextMessageSendDelegate::UpdateContactInfo);
    }
#endif

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

bool CTextMessageSendDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /*= NULL*/)
{
    if (NULL == g_pTextMessageController || MSGBOX_ID_SEND != nID)
    {
        return false;
    }
    g_SetUICallBackFun.FallBackToPrevousPage();
    g_SetUICallBackFun.FallBackToPrevousPage();
    g_pTextMessageController->ResetTempTextMsgDetailData();
    return true;
}

bool CTextMessageSendDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    //处理进入发送界面，当按下ok键，处理同softkeysend按键一样
    if (PHONE_KEY_OK == nKeyCode)
    {
        yl::string strSendTo = GetValue(ITEMID_TEXTMESSAGE_SEND_TO);
        int nAccountId = GetComboboxValue(ITEMID_TEXTMESSAGE_SEND_FORM);
        if (INVALID_ACCOUNTID == nAccountId)
        {
            CSettingUIDataValidInfo dataValid;
            int nDataIndex = GetItemDataIndexByItemId(ITEMID_TEXTMESSAGE_SEND_TO);
            dataValid.SetValidIndex(nDataIndex);
            dataValid.SetValidNote(_LANG(TRID_INVALID_ACCOUNT));
            g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        }
        else if (strSendTo.empty())
        {
            CSettingUIDataValidInfo dataValid;
            int nDataIndex = GetItemDataIndexByItemId(ITEMID_TEXTMESSAGE_SEND_TO);
            dataValid.SetValidIndex(nDataIndex);
            dataValid.SetValidNote(_LANG(TRID_TARGET_CANNOT_BE_EMPTY));
            g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        }
        else
        {
            TextMsgItemDetailData stDetailData = g_pTextMessageController->GetTempTextMsgDetailData();
            TextMsgSendItemData stMsgSendData;
            stMsgSendData.m_nFromAccountId = nAccountId;
            stMsgSendData.m_strTo = strSendTo;
            stMsgSendData.m_tTime = netGetLocalTime();
            stMsgSendData.m_strText = stDetailData.m_strText;
            g_pTextMessageController->SendTheMessage(stMsgSendData);
            if (TEXT_MSG_GROUP_DRAFT == stDetailData.m_eGType)
            {
                int nMessageIndex = g_pTextMessageController->GetTempDataIndex();
                // 删除草稿信息
                g_pTextMessageController->RemoveMessage(TEXT_MSG_GROUP_DRAFT, nMessageIndex);
            }
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_SENDING_MESSAGE)
                                                   , SETTING_MESSAGEBOX_NOTE, "", "", NOTE_MSGBOX_SHOW_TIME
                                                   , MSGBOX_ID_SEND);
        }
        return true;
    }

    return false;
}

bool CTextMessageSendDelegate::OnBackAction()
{
    g_SetUICallBackFun.CacheDlgData(m_strPageID);
    return true;
}
