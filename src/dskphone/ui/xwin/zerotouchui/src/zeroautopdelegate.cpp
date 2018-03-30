#include "zeroautopdelegate.h"
#include "settinguilogic/include/settingdefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/settinguidialogmanager.h"
#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/dataloader.h"
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/include/zero_common.h"
#include "common/common_data_define.h"
#include "translateiddefine.h"
#include "feature/include/modfeature.h"

#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "settingui/src/uicallbackfun.h"
#include <configiddefine.h>
#include "commonunits/modcommonunits.h"
#include "settinguilogic/src/settinguilineedititemdata.h"

namespace
{
#define   SOFT_KEY_BACK_INDEX   0
#define   SOFT_KEY_OK_INDEX     3
#define   DEFAULT_IME_PWD_abc       "abc_PWD"
#define   DEFAULT_IME_PWD_2aB       "2aB_PWD"

}

CZeroAutopDelegate::CZeroAutopDelegate()
    : CSettinguiDataDelegateBase()
    , m_bAutoPCodeAuth(false)
{
}

CZeroAutopDelegate::~CZeroAutopDelegate()
{
}

CSettingUIDelegateBase * CZeroAutopDelegate::CreateZeroAutopDelegate()
{
    CZeroAutopDelegate * pStatusDelegate = new CZeroAutopDelegate;

    return (CSettingUIDelegateBasePtr)pStatusDelegate;
}

bool CZeroAutopDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
                                      bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    CSettingUIDelegateBase::SavePageData(pPageData, false, false);
    return true;
}

void CZeroAutopDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    vecSoftKey[0].m_iType = ST_BACK;
    vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);

    vecSoftKey[3].m_iType = ST_NEXT;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_OK);
}

bool CZeroAutopDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (!bLongPress && ST_NEXT == iType)
    {
        return OnOKAction(bLongPress, nDataIndex);
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

bool CZeroAutopDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
                                        bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return false;
    }

    if (m_bAutoPCodeAuth)
    {
        SetItemShow(pPagedata, AUTOP_CODE_URL, false);
    }

    return true;
}

void CZeroAutopDelegate::InitExtraData(void * pData)
{
    ZEROUI_INFO("CZeroAutopDelegate::InitExtraData pData is %p.", pData);
    if (NULL == pData)
    {
        return;
    }

    m_bAutoPCodeAuth = *((bool *)pData);
    ZEROUI_INFO("CZeroAutopDelegate::InitExtraData m_bAutoPCodeAuth is %d.", m_bAutoPCodeAuth);
}

CSettingUIPageDataBase * CZeroAutopDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (m_bAutoPCodeAuth)
    {
        m_pPageData->SetPageTitle(_LANG(TRID_AUTOP_AHTHORITY));
    }
    else
    {
        m_pPageData->SetPageTitle(_LANG(TRID_AUTO_PROVISION));
    }

    if (NULL != m_pPageData && bReLoad && m_bAutoPCodeAuth)
    {
        SetValue(AUTOP_CODE_USERNAME, "");
        SetValue(AUTOP_CODE_PSW, "");
    }

    yl::string strDefaultImePwd = DEFAULT_IME_PWD_abc;
    if (m_bAutoPCodeAuth)
    {
        strDefaultImePwd = DEFAULT_IME_PWD_2aB;
    }
    else
    {
#if IF_FEATURE_AUTOP_IME
        CSettingUILineEditItemDataPtr pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(m_pPageData->GetItemByItemId(AUTOP_CODE_USERNAME));
        if (NULL != pEditItem)
        {
            yl::string strImeType = commonUnits_GetIMEByType(kszZeroUserIME);
            pEditItem->SetDefaultIME(strImeType);
        }
        strDefaultImePwd = commonUnits_GetIMEByType(kszZeroPasswordIME);
#endif
    }
    CSettingUILineEditItemDataPtr pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(m_pPageData->GetItemByItemId(AUTOP_CODE_PSW));
    if (NULL != pEditItem)
    {
        pEditItem->SetDefaultIME(strDefaultImePwd);
    }

    return m_pPageData;
}

bool CZeroAutopDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (PHONE_KEY_OK == nKeyCode && bPress)
    {
        return OnOKAction(false, GetFocusIndex());
    }
    return false;
}

bool CZeroAutopDelegate::OnOKAction(bool bLongPress, int nDataIndex)
{
    if (m_bAutoPCodeAuth)
    {
        yl::string strUserName = GetValue(AUTOP_CODE_USERNAME);
        yl::string strPassword = GetValue(AUTOP_CODE_PSW);
        // liuqy TBD Autop ½Ó¿ÚÌæ»»
        //feature_AutopCodeTryToConnect(strUserName.c_str(), strPassword.c_str());
        g_SetUICallBackFun.FallBackToPrevousPage();
        return true;
    }
    else if (CSettingUIDelegateBase::OnSoftkeyAction(ST_SAVE, bLongPress, nDataIndex))
    {
        CSettingDialogBase* pDlg = g_pSettingUIDialogManager->GetTopActiveDialog(m_strPageID);
        if (NULL == pDlg)
        {
            return false;
        }

        g_SetUICallBackFun.DropOutSettingUI();
        SettingUILogic_EnterZeroNextStage(ZERO_AUTOP_PAGE, ZERO_NEXT);
        return true;
    }
    return false;
}

