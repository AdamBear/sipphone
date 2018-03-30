#include "hotelingguestunlockcheckdelegate.h"
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

namespace
{
#define MSGBOX_ID_PING_ERROR                1001
}

CHotelingGuestUnlockCheckDelegate::CHotelingGuestUnlockCheckDelegate()
{

}

CHotelingGuestUnlockCheckDelegate::~CHotelingGuestUnlockCheckDelegate()
{
}

CSettingUIDelegateBase * CHotelingGuestUnlockCheckDelegate::CreateHotelingGuestUnlockCheckDelegate()
{
    CHotelingGuestUnlockCheckDelegate * pDelegate = new CHotelingGuestUnlockCheckDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CHotelingGuestUnlockCheckDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData)
    {
        return true;
    }

    yl::string strUnlock = GetValue(ITEM_GUEST_UNLOCK_CHECK_EDIT);

    if (CallCenter_IsMatchGuestLockPin(strUnlock))
    {
        CallCenter_GuestLock(false);
        // 退出PIN校验界面
        g_SetUICallBackFun.FallBackToPrevousPage();
    }
    else
    {
        // 弹出提示
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING),
                                               _LANG(TRID_UNLOCK_PIN_INCORRECT)
                                               , SETTING_MESSAGEBOX_OkCancel, "", "", SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME
                                               , MSGBOX_ID_PING_ERROR, NULL);
    }

    return true;
}

bool CHotelingGuestUnlockCheckDelegate::OnAction(int eActionType, int iDataIndex)
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

bool CHotelingGuestUnlockCheckDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /*= NULL*/)
{
    if (MSGBOX_ID_PING_ERROR == nID)
    {
        if (SETTING_MessageBox_OK == nResult
                || SETTING_MESSAGEBOX_OkCancel == nResult
                || SETTING_MessageBox_TimeOut == nResult)
        {
            yl::string strValue = "";
            SetValue(ITEM_GUEST_UNLOCK_CHECK_EDIT, strValue);
            g_SetUICallBackFun.RefreshUI(false, GetPageID());
        }
        return true;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

