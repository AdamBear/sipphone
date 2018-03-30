#include "settinguiphoneunlockcheckdelegate.h"

#if !IF_UNLOCK_CHECK_POPBOX
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "baseui/include/baseuicommon.h"
#include "keyguard/include/modkeyguard.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include <configiddefine.h>
#include "commonunits/modcommonunits.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguimanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "keyguard/include//keyguardmessagedefine.h"

namespace
{
#define MSGBOX_ID_PING_ERROR 1001
}

CSettinguiPhoneUnlockCheckDelegate::CSettinguiPhoneUnlockCheckDelegate()
    : m_bEnterBySetting(true)
{
    etl_RegisterMsgHandle(KEYGUARD_ENABLE_CHANGED, KEYGUARD_ENABLE_CHANGED,
                          &CSettinguiPhoneUnlockCheckDelegate::OnMessage);
}

CSettinguiPhoneUnlockCheckDelegate::~CSettinguiPhoneUnlockCheckDelegate()
{
    etl_UnregisterMsgHandle(KEYGUARD_ENABLE_CHANGED, KEYGUARD_ENABLE_CHANGED,
                            &CSettinguiPhoneUnlockCheckDelegate::OnMessage);
}

bool CSettinguiPhoneUnlockCheckDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /* = NULL */)
{
    if (MSGBOX_ID_PING_ERROR == nID)
    {
        if (SETTING_MessageBox_OK == nResult
                || SETTING_MESSAGEBOX_OkCancel == nResult
                || SETTING_MessageBox_TimeOut == nResult)
        {
            yl::string strValue = "";
            SetValue(ITEM_PHONE_UNLOCK_CHECK_EDIT, strValue);
            g_SetUICallBackFun.RefreshUI(false, GetPageID());
        }

        return true;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

CSettingUIDelegateBase * CSettinguiPhoneUnlockCheckDelegate::CreatePhoneUnlockCheckDelegate()
{
    CSettinguiPhoneUnlockCheckDelegate * pDelegate = new CSettinguiPhoneUnlockCheckDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettinguiPhoneUnlockCheckDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    return true;
}

bool CSettinguiPhoneUnlockCheckDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        if (!IsPingRight())
        {
            // 弹出提示
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING),
                                                   _LANG(TRID_CURRENT_PIN_IS_INCORRECT)
                                                   , SETTING_MESSAGEBOX_OkCancel, "", "", NS_SET::g_SetPopNormalTime
                                                   , MSGBOX_ID_PING_ERROR, NULL);
        }
        return true;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

bool CSettinguiPhoneUnlockCheckDelegate::IsPingRight()
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    yl::string strPing = GetValue(ITEM_PHONE_UNLOCK_CHECK_EDIT);

    PinType ePinType = keyGuard_GetPinType(strPing);

    PageOpenData opPageData;
    // 有回退界面，会删除delegate，先缓存数据
    bool bEnterBySetting = m_bEnterBySetting;

    if (PIN_INCORRECT == ePinType)
    {
        return false;
    }
    else if (PIN_CORRECT_ADMIN == ePinType)
    {
        // 输入了admin密码,解锁密码
        keyGuard_Lock(false);
        // 退出PIN校验界面
        g_SetUICallBackFun.FallBackToPrevousPage();
#if IF_BUG_21722
        if (KG_MENU_KEY == keyGuard_GetKeyGuardType())
        {
            UIManager_ReturnToIdle();
        }
#else
        // basic setting进入phone lock, 输入admin密码就可以进入PAGE_ADV_LOCK界面
        if (bEnterBySetting)
        {
            opPageData.strPageAction = kszPageIDPhoneLock;
            g_SetUICallBackFun.EnterPage(opPageData);
        }
#endif
        // 进入重置密码界面
        bool bPwdVerified = true;
        opPageData.strPageAction = kszPageIDChangePin;
        opPageData.pExtraData = (void *)(&bPwdVerified);
        g_SetUICallBackFun.EnterPage(opPageData);
    }
    else if (PIN_CORRECT_LOCK == ePinType)
    {
        // 普通解锁密码验证通过,解锁并退出界面
        keyGuard_Lock(false);
        // 退出PIN校验界面
        g_SetUICallBackFun.FallBackToPrevousPage();
#if IF_BUG_21722
        if (KG_MENU_KEY == keyGuard_GetKeyGuardType())
        {
            UIManager_ReturnToIdle();
            opPageData.strPageAction = kszPageIDMainMenu;
            g_SetUICallBackFun.EnterPage(opPageData);
        }
#else
        if (bEnterBySetting)
        {
            opPageData.strPageAction = kszPageIDPhoneLock;
            g_SetUICallBackFun.EnterPage(opPageData);
        }
#endif
    }

    return true;
}

void CSettinguiPhoneUnlockCheckDelegate::InitExtraData(void * pData)
{
    if (NULL == pData)
    {
        return;
    }
    m_bEnterBySetting = *((bool *)pData);
}

CSettingUIPageDataBase * CSettinguiPhoneUnlockCheckDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

#if IF_FEATURE_AUTOP_IME
    CSettingUILineEditItemDataPtr pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(
                m_pPageData->GetItemByItemId(ITEM_PHONE_UNLOCK_CHECK_EDIT));
    if (NULL != pEditItem)
    {
        yl::string strImeType = commonUnits_GetIMEByType(kszUnlockPinIME);
        pEditItem->SetDefaultIME(strImeType);
    }
#endif

    return m_pPageData;
}

LRESULT CSettinguiPhoneUnlockCheckDelegate::OnMessage(msgObject & msg)
{
    BOOL bRet = FALSE;
    CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(
            kszPageIDPhoneUnlockCheck);
    CSettinguiPhoneUnlockCheckDelegate * pUnlockDelegate =
        static_cast<CSettinguiPhoneUnlockCheckDelegate *>(pDelegate);
    if (NULL == pUnlockDelegate || pUnlockDelegate != g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }
    switch (msg.message)
    {
    case KEYGUARD_ENABLE_CHANGED:
        {
            bool bLock = (bool)msg.wParam;
            if (!bLock && !pUnlockDelegate->IsEnterBySetting())
            {
                // 退出PIN校验界面
                g_SetUICallBackFun.FallBackToPrevousPage();
            }
            bRet = TRUE;
        }
        break;
    default:
        break;
    }
    return bRet;
}

bool CSettinguiPhoneUnlockCheckDelegate::IsEnterBySetting()
{
    return m_bEnterBySetting;
}
#endif
