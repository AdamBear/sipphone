#include "hotelinglogindelegate.h"
#include "setuidatadefine.h"
#include "settingui/include/itemnamedefine.h"
#include "translateiddefine.h"
#include "settingui/include/pageiddefine.h"

#include "messagebox/modmessagebox.h"
#include "languagehelper/modlangtranslate.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include <configiddefine.h>
#include "commonunits/modcommonunits.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/include/common.h"

namespace
{
#define HOTELING_PASSWORD_STRING            "******"
#define NOTE_MSGBOX_SHOW_TIME_LOGINING      -1
#define MSGBOX_ID_LOGINING                  100
}

CHotelingLoginDelegate::CHotelingLoginDelegate()
    : m_nHotelingType(0)
{
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_CHANGE
                          , &CHotelingLoginDelegate::OnMessage);
    etl_RegisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE
                          , &CHotelingLoginDelegate::OnMessage);
    etl_RegisterMsgHandle(HOTELING_MESSAGE_REQ_TIMEOUT, HOTELING_MESSAGE_REQ_TIMEOUT
                          , &CHotelingLoginDelegate::OnMessage);
    etl_RegisterMsgHandle(HOTELING_MESSAGE_REQ_FAIL, HOTELING_MESSAGE_REQ_FAIL
                          , &CHotelingLoginDelegate::OnMessage);
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_UPDATE, ACD_MESSAGE_STATUS_UPDATE
                          , &CHotelingLoginDelegate::OnMessage);
}

CHotelingLoginDelegate::~CHotelingLoginDelegate()
{
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_CHANGE
                            , &CHotelingLoginDelegate::OnMessage);
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE
                            , &CHotelingLoginDelegate::OnMessage);
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_REQ_TIMEOUT, HOTELING_MESSAGE_REQ_TIMEOUT
                            , &CHotelingLoginDelegate::OnMessage);
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_REQ_FAIL, HOTELING_MESSAGE_REQ_FAIL
                            , &CHotelingLoginDelegate::OnMessage);
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_UPDATE, ACD_MESSAGE_STATUS_UPDATE
                            , &CHotelingLoginDelegate::OnMessage);
}

CSettingUIDelegateBase * CHotelingLoginDelegate::CreateHotelingLoginDelegate()
{
    CHotelingLoginDelegate * pDelegate = new CHotelingLoginDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CHotelingLoginDelegate::InitExtraData(void * pData)
{
    if (NULL == pData)
    {
        return;
    }

    m_nHotelingType = *((int *)pData);
}

CSettingUIPageDataBase * CHotelingLoginDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData)
    {
        return NULL;
    }

    if (CallCenter_IsFlexibleSeatingOn())
    {
        m_pPageData->SetPageTitle(_LANG(TRID_FLEXSEAT_LOGIN));
    }
    else
    {
        m_pPageData->SetPageTitle(_LANG(TRID_GUEST_LOGIN));
    }

    // UserId
    yl::string strUserId;
    Hoteling_GetUserId(strUserId);
    SetValue(HOTELING_USERID, strUserId);

    // password
    if (Hoteling_IsAutoLogin())
    {
        SetValue(HOTELING_PASSWORD, HOTELING_PASSWORD_STRING);
    }

#if IF_FEATURE_AUTOP_IME
    CSettingUILineEditItemDataPtr pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(
                m_pPageData->GetItemByItemId(HOTELING_USERID));
    if (NULL != pEditItem)
    {
        yl::string strImeType = commonUnits_GetIMEByType(kszHotelingUserIME);
        pEditItem->SetDefaultIME(strImeType);
    }

    pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(m_pPageData->GetItemByItemId(
                    HOTELING_PASSWORD));
    if (NULL != pEditItem)
    {
        yl::string strImeType = commonUnits_GetIMEByType(kszHotelingPasswordIME);
        pEditItem->SetDefaultIME(strImeType);
    }
#endif

    SetComboboxValue(HOTELING_AUTOLOGIN, Hoteling_IsAutoLogin());

    if (m_nHotelingType == HOTELING_TYPE_ACDON && !CallCenter_IsFlexibleSeatingOn())
    {
        SetComboboxValue(HOTELING_USERHOST, false);
    }

    return m_pPageData;
}

bool CHotelingLoginDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData)
    {
        return true;
    }

    ACDLogin        stACDLoginData;
    yl::string      strPwd;
    stACDLoginData.m_strUser = GetValue(HOTELING_USERID);
    strPwd = GetValue(HOTELING_PASSWORD);
    if (strPwd != HOTELING_PASSWORD_STRING)
    {
        stACDLoginData.m_strPassword = strPwd;
    }
    else
    {
        stACDLoginData.m_strPassword = "";
    }

    if (0 == GetComboboxValue(HOTELING_AUTOLOGIN))
    {
        stACDLoginData.m_bIsAuto = false;
    }
    else
    {
        stACDLoginData.m_bIsAuto = true;
    }

    if (m_nHotelingType == HOTELING_TYPE_ACDOFF || CallCenter_IsFlexibleSeatingOn())
    {
        if (0 == GetComboboxValue(HOTELING_USERHOST))
        {
            stACDLoginData.m_bIsUseHost = false;
        }
        else
        {
            stACDLoginData.m_bIsUseHost = true;
        }
    }

    if (stACDLoginData.m_strUser.empty() &&
            (m_nHotelingType == HOTELING_TYPE_ACDOFF || !stACDLoginData.m_bIsUseHost))
    {
        CSettingUIDataValidInfo dataValid;
        int nDataIndex = GetItemDataIndexByItemId(HOTELING_USERID);
        dataValid.SetValidIndex(nDataIndex);
        dataValid.SetValidNote(_LANG(TRID_USER_ID_CANNOT_BE_EMPTY));
        g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        return true;
    }

    //登录Hoteling
    if (CallCenter_Login(stACDLoginData))
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_LOGINING)
                                               , SETTING_MESSAGEBOX_NOTE, "", "", NOTE_MSGBOX_SHOW_TIME_LOGINING, MSGBOX_ID_LOGINING, NULL, this);
    }
    else
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING), _LANG(TRID_LOGIN_FAILED)
                                               , SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
    }

    return true;
}

bool CHotelingLoginDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        eActionType = SET_ACTION_TYPE_SAVE;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

LRESULT CHotelingLoginDelegate::OnMessage(msgObject & msg)
{
    CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDHotelingLogin);
    CHotelingLoginDelegate * pHotelDelegate = static_cast<CHotelingLoginDelegate *>(pDelegate);
    if (NULL == pHotelDelegate || pHotelDelegate != g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    switch (msg.message)
    {
    case HOTELING_MESSAGE_STATUS_CHANGE:
    case ACD_MESSAGE_STATUS_CHANGE:
        {
            MessageBox_RemoveMessageBox(pHotelDelegate);
            g_SetUICallBackFun.FallBackToPrevousPage();
        }
        break;
    case HOTELING_MESSAGE_REQ_TIMEOUT:
    case HOTELING_MESSAGE_REQ_FAIL:
        {
            MessageBox_RemoveMessageBox(pHotelDelegate);
            g_SetUICallBackFun.PopCommonMessageBox(pDelegate->GetPageID(), _LANG(TRID_WARNING),
                                                   _LANG(TRID_LOGIN_FAILED)
                                                   , SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
        }
        break;
    default:
        break;
    }
    return TRUE;
}

void CHotelingLoginDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    // 由于收到消息需要主动移除提示的登录中弹框，要派生自CMsgBoxCallBackAction
    // CMsgBoxCallBackAction类中该接口是纯虚函数，需要真正实现该函数
}

bool CHotelingLoginDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);
    if (m_nHotelingType == HOTELING_TYPE_ACDOFF || CallCenter_IsFlexibleSeatingOn())
    {
        SetItemShow(m_pPageData, HOTELING_USERHOST, false);
    }
    //在xmlbrower下，过滤userhost和autologin
    else if (m_nHotelingType == HOTELING_TYPE_XMLBROWSER)
    {
        SetItemShow(m_pPageData, HOTELING_USERHOST, false);
        SetItemShow(m_pPageData, HOTELING_AUTOLOGIN, false);
    }

    return bResult;
}
