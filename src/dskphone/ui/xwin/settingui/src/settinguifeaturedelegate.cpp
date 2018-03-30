#include "settinguifeaturedelegate.h"
#include "settingui/include/pageiddefine.h"
#include "uicallbackfun.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "account/include/modaccount.h"

CSettingUIFeatureDelegate::CSettingUIFeatureDelegate()
{

}

CSettingUIFeatureDelegate::~CSettingUIFeatureDelegate()
{

}

CSettingUIDelegateBase * CSettingUIFeatureDelegate::CreateFeatureDelegate()
{
    CSettingUIFeatureDelegate * pDelegate = new CSettingUIFeatureDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettingUIFeatureDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /*= false*/)
{
    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    if (NULL == pPagedata)
    {
        return bResult;
    }

#ifdef IF_FEATURE_BROADSOFT_BASE
    //Brosoft和xsi同时开启下才显示Call Controll
    if (1 == configParser_GetConfigInt(kszBroadsoftActive)
            && 1 == configParser_GetConfigInt(kszXSISwitch))
    {
        SetItemShow(pPagedata, MAINMENU_CALLCONTROL, true);
    }
    else
#endif
    {
        SetItemShow(pPagedata, MAINMENU_CALLCONTROL, false);
    }

    bool bShowDnd = true;
    bool bShowFoward = true;
    bool bShowCallWaiting = true;
    bool bShowAnonymous = true;

    if (1 == configParser_GetConfigInt(kszDndForbidden))
    {
        bShowDnd = false;
    }

    if (1 == configParser_GetConfigInt(kszForwardForbidden))
    {
        bShowFoward = false;
    }

    if (1 == configParser_GetConfigInt(kszBroadsoftActive))
    {
#ifdef IF_FEATURE_BROADSOFT_BASE
        if (0 != configParser_GetConfigInt(kszXSISwitch))
        {
            bShowDnd = false;
#if !IF_BUG_23784
            bShowFoward = false;
#if IF_BUG_30078
            bShowAnonymous = false;
#endif //IF_BUG_30078
#endif //IF_BUG_23784
        }
#endif //IF_FEATURE_BROADSOFT_BASE

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
        if (1 == configParser_GetConfigInt(kszCallWaitingMode))
        {
            bShowCallWaiting = false;
        }
#endif
    }

#if IF_FEATURE_METASWITCH
    if (1 == configParser_GetConfigInt(kszMTSWActive))
    {
        bShowAnonymous = false;
    }
#endif

#ifdef IF_FEATURE_PSTN
    if (!netIsCableConnected())
    {
        bShowFoward = false;
        bShowCallWaiting = false;
        bShowAnonymous = false;
#ifndef IF_FEATURE_PSTN_DND
        bShowDnd = false;
#endif
        SetItemShow(pPagedata, FEATURES_INTERCOM, false);
        SetItemShow(pPagedata, FEATURES_CALL_COMPLETION, false);
        SetItemShow(pPagedata, FEATURES_PAGING_LIST, false);
        SetItemShow(pPagedata, FEATURES_AUTORD, false);
        SetItemShow(pPagedata, FEATURES_DIAGNOSTICS, false);
    }
#endif

    SetItemShow(pPagedata, FEATURE_DND, bShowDnd);
    SetItemShow(pPagedata, FEATURE_CALL_FORWARD, bShowFoward);
    SetItemShow(pPagedata, FEATURE_CALL_WAITING, bShowCallWaiting);
    SetItemShow(pPagedata, FEATURE_ANONYMOUS, bShowAnonymous);

    return bResult;
}

void CSettingUIFeatureDelegate::OnEnterPage(const yl::string & strPage, int iDataIndex)
{
    if (kszPageIDAnonymousCall == strPage)
    {
        PageOpenData opPageData(strPage);
        int iLineId = GetAvailableLineID();
        opPageData.pExtraData = (void *)&iLineId;
        g_SetUICallBackFun.EnterPage(opPageData);
    }
    else if (kszPageIDDNDLineSelect == strPage && 0 == configParser_GetConfigInt(kszDndMode))
    {
        PageOpenData opPageData(kszPageIDPhoneDND);
        g_SetUICallBackFun.EnterPage(opPageData);
        return;
    }
    else if (kszPageIDCallForward == strPage && 1 == configParser_GetConfigInt(kszForwardMode))
    {
        PageOpenData opPageData(kszPageIDCallForwardLineSelect);
        g_SetUICallBackFun.EnterPage(opPageData);
        return;
    }

    return CSettingUIDelegateBase::OnEnterPage(strPage, iDataIndex);
}

int CSettingUIFeatureDelegate::GetAvailableLineID()
{
    int nTotalAccount = acc_AccountNum();
    for (int i = 0; i < nTotalAccount; i++)
    {
        if (acc_IsAccountAvailable(i))
        {
            return i;
        }
    }

    return -1;
}
