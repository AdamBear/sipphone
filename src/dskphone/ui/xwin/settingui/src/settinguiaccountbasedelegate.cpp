#include "settinguiaccountbasedelegate.h"
#include "settinguilogic/src/settinguibaseaccountpagedata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "common/common_data_define.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettingUIAccountDelegateBase::CSettingUIAccountDelegateBase()
{
}

CSettingUIAccountDelegateBase::~CSettingUIAccountDelegateBase()
{
}

CSettingUIDelegateBase * CSettingUIAccountDelegateBase::CreateAccountDelegateBase()
{
    CSettingUIAccountDelegateBase * pDelegate = new CSettingUIAccountDelegateBase;

    return (CSettingUIAccountDelegateBasePtr)pDelegate;
}

void CSettingUIAccountDelegateBase::InitExtraData(void * pData)
{
    CSettingUIDelegateBase::InitExtraData(pData);

    if (NULL != pData)
    {
        int * iAccountID = (int *)pData;
        if (NULL != iAccountID)
        {
            m_iLineID = *iAccountID;
        }
    }
}


CSettingUIPageDataBase * CSettingUIAccountDelegateBase::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData || !bReLoad)
    {
        return m_pPageData;
    }

    CSettingUIAccountPageDataBasePtr pAccountPageData =
        CSettingUIAccountPageDataBase::GetAccountPageData(m_pPageData);
    if (NULL != pAccountPageData)
    {
        pAccountPageData->SetLineID(m_iLineID);
#if IF_SETTING_DISPLAY_DEBUG
        SETTINGUI_INFO("#$#$#$#$#$#$$$$$$$$$$$$$$$$ set LineID [%d] =======", m_iLineID);
#endif
    }

    return m_pPageData;
}

void CSettingUIAccountDelegateBase::ShowOnlyBackSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    int nSize = vecSoftKey.size();
    for (int i = 0; i < nSize; ++i)
    {
        if (0 == i)
        {
            vecSoftKey[0].m_iType = ST_BACK;
            vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);
        }
        else
        {
            vecSoftKey[i].m_iType = ST_EMPTY;
            vecSoftKey[i].m_strSoftkeyTitle = "";
        }
    }
}
