#include "settinguiadvpstnaccountsettingdelegate.h"

#ifdef IF_FEATURE_PSTN

#include "setuidatadefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CAdvPSTNAccountSettingDelegate::CAdvPSTNAccountSettingDelegate()
{

}

CAdvPSTNAccountSettingDelegate::~CAdvPSTNAccountSettingDelegate()
{

}

CSettingUIDelegateBase * CAdvPSTNAccountSettingDelegate::CreateAdvPSTNAccountSettingDelegate()
{
    CAdvPSTNAccountSettingDelegate * pDelegate = new CAdvPSTNAccountSettingDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CAdvPSTNAccountSettingDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (NULL != m_pPageData && bReLoad)
    {
        SetComboboxValue(ADV_PSTN_ACCOUNT_ACTIVE, acc_IsAccountEnable(m_iLineID));
        SetValue(ADV_PSTN_ACCOUNT_LABEL, acc_GetAccountLabel(m_iLineID));
    }

    return m_pPageData;
}

bool CAdvPSTNAccountSettingDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    bool bEnable = (1 == GetComboboxValue(ADV_PSTN_ACCOUNT_ACTIVE));
    yl::string strLabel = GetValue(ADV_PSTN_ACCOUNT_LABEL);

    bool bChange = false;
    bChange |= acc_SetAccountEnable(m_iLineID, bEnable);
    bChange |= acc_SetAccountLabel(m_iLineID, strLabel);

    int nMsgBoxID = MSGBOX_ID_SAVE;
    if (bChange)
    {
        nMsgBoxID = MSGBOX_ID_SAVE_AND_REFRESH;
        //异步通知账号配置改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, m_iLineID);
    }

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), nMsgBoxID);

    return true;
}

#endif
