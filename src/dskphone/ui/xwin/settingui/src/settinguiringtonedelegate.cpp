#include "settinguiringtonedelegate.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguimenuitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/src/soundcontroller.h"

CSettingUIRingToneDelegate::CSettingUIRingToneDelegate()
{

}

CSettingUIRingToneDelegate::~CSettingUIRingToneDelegate()
{

}

CSettingUIDelegateBase * CSettingUIRingToneDelegate::CreateRingToneDelegate()
{
    CSettingUIRingToneDelegate * pDelegate = new CSettingUIRingToneDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSettingUIRingToneDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        AddListItem(m_pPageData);
    }

    return m_pPageData;
}

bool CSettingUIRingToneDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        PageOpenData opPageData(kszPageIDRingToneSetting);
        int nAccountID = GetRealAccountID(iDataIndex);
        opPageData.pExtraData = (void *) & (nAccountID);
        g_SetUICallBackFun.EnterPage(opPageData);
        return true;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

void CSettingUIRingToneDelegate::AddListItem(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return ;
    }

    //Common
    AddRingToneMenuItem(pPageData, _LANG(TRID_COMMON_RINGTONE), -1);

    // Account
    int nTotalAccount = acc_AccountNum();
    for (int i = 0; i < nTotalAccount; i++)
    {
        if (acc_IsAccountAvailable(i))
        {
            AddRingToneMenuItem(pPageData, acc_GetAccountShowText(i), i);
        }
    }

#ifdef IF_FEATURE_PSTN
    //PSTN
    ListAccounts listAccountId = acc_GetAvailableAccountList(ACCOUNT_TYPE_PSTN);
    ListAccounts::ListIterator iter = listAccountId.begin();
    for (; iter != listAccountId.end(); ++iter)
    {
        int iAccountId = *iter;
        AddRingToneMenuItem(pPageData, acc_GetLabel(iAccountId), iAccountId);
    }
#endif
}

void CSettingUIRingToneDelegate::AddRingToneMenuItem(CSettingUIPageDataBase * pPageData,
        const yl::string & strDisp, int nAccountID)
{
    if (NULL == pPageData)
    {
        return;
    }

    const yl::string & strItemID = commonAPI_FormatString("%d", nAccountID + 1);
    AddMenuItem(pPageData, strItemID, strDisp);
}

int CSettingUIRingToneDelegate::GetRealAccountID(int nDataIndex)
{
    yl::string strItemID = GetItemIDByDataIndex(nDataIndex);
    if (!strItemID.empty())
    {
        return atoi(strItemID.c_str());
    }

    return 0;
}
