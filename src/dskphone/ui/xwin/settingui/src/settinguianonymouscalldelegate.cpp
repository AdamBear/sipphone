#include "settinguianonymouscalldelegate.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "baseui/include/baseuicommon.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguidatavalidinfo.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/featurecontroller.h"
#include "account/include/modaccount.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define ITEM_CALL_OFFCODE_ID 5
#define ITEM_REJECTION_OFFCODE_ID 8
}

CSettinguiAnonymousCallDelegate::CSettinguiAnonymousCallDelegate()
{

}

CSettinguiAnonymousCallDelegate::~CSettinguiAnonymousCallDelegate()
{

}

CSettingUIDelegateBase * CSettinguiAnonymousCallDelegate::CreateAnonymousCallDelegate()
{
    CSettinguiAnonymousCallDelegate * pDelegate = new CSettinguiAnonymousCallDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSettinguiAnonymousCallDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData || !bReLoad)
    {
        return m_pPageData;
    }

    AnonymousCallData stAnonymousCallData;
    if (GetAnonymousCallDataByLineId(&stAnonymousCallData, m_iLineID))
    {
        SetValue(ANONYMOUS_LINEID, acc_GetAccountShowText(m_iLineID));
        SetComboboxValue(ANONYMOUS_CALLENABLE, stAnonymousCallData.m_isAnonymousCallOn);
        SetComboboxValue(ANONYMOUS_REJECTION_ENABLE, stAnonymousCallData.m_isRejectAnonymousCallOn);

        SetComboboxValue(ANONYMOUS_SEND_ENABLE, stAnonymousCallData.m_isAnonymousSendCode);
        SetValue(ANONYMOUS_CALL_ONCODE, stAnonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOn);
        SetValue(ANONYMOUS_CALL_OFFCODE, stAnonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOff);

        SetComboboxValue(ANONYMOUS_REJECTION_SEND_ENABLE, stAnonymousCallData.m_isAnonymousRejectSendCode);
        SetValue(ANONYMOUS_REJECTION_ONCODE,
                 stAnonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn);
        SetValue(ANONYMOUS_REJECTION_OFFCODE,
                 stAnonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff);
    }

    return m_pPageData;
}

bool CSettinguiAnonymousCallDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIAccountDelegateBase::PageDataFilter(pPagedata, bReload);

    if (-1 == m_iLineID)
    {
        HideAllItem(pPagedata);
        return bResult;
    }

    //broadsoft开启之后不显示code
    if (1 == configParser_GetConfigInt(kszBroadsoftActive)
#if IF_FEATURE_METASWITCH
            || configParser_GetConfigInt(kszMetaSwitchActive) != 0
#endif
       )
    {
        SetItemShow(pPagedata, ANONYMOUS_SEND_ENABLE);
        SetItemShow(pPagedata, ANONYMOUS_CALL_ONCODE);
        SetItemShow(pPagedata, ANONYMOUS_CALL_OFFCODE);
        SetItemShow(pPagedata, ANONYMOUS_REJECTION_SEND_ENABLE);
        SetItemShow(pPagedata, ANONYMOUS_REJECTION_ONCODE);
        SetItemShow(pPagedata, ANONYMOUS_REJECTION_OFFCODE);
    }

    return bResult;
}

bool CSettinguiAnonymousCallDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == m_pPageData || !pPageData->IsEnable())
    {
        return true;
    }

    AnonymousCallData stAnonymousCallData;

    GetPageData(stAnonymousCallData);

    CSettingUIDataValidInfo dataValid;
    if (!IsPageDataValid(stAnonymousCallData, dataValid))
    {
        g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        return true;
    }

    SetAnonymousCallDataByLineId(m_iLineID, stAnonymousCallData);

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE);

    return true;
}

void CSettinguiAnonymousCallDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (-1 != m_iLineID)
    {
        return;
    }

    ShowOnlyBackSoftkey(vecSoftKey);
}

void CSettinguiAnonymousCallDelegate::GetPageData(AnonymousCallData & stAnonymousCallData)
{
    stAnonymousCallData.m_isAnonymousCallOn = GetComboboxValue(ANONYMOUS_CALLENABLE);
    stAnonymousCallData.m_isRejectAnonymousCallOn = GetComboboxValue(ANONYMOUS_REJECTION_ENABLE);

    stAnonymousCallData.m_isAnonymousSendCode = GetComboboxValue(ANONYMOUS_SEND_ENABLE);
    strcpy(stAnonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOn,
           GetValue(ANONYMOUS_CALL_ONCODE).c_str());
    strcpy(stAnonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOff,
           GetValue(ANONYMOUS_CALL_OFFCODE).c_str());

    stAnonymousCallData.m_isAnonymousRejectSendCode = GetComboboxValue(ANONYMOUS_REJECTION_SEND_ENABLE);
    strcpy(stAnonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn,
           GetValue(ANONYMOUS_REJECTION_ONCODE).c_str());
    strcpy(stAnonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff,
           GetValue(ANONYMOUS_REJECTION_OFFCODE).c_str());
}

bool CSettinguiAnonymousCallDelegate::IsPageDataValid(const AnonymousCallData & stData,
        CSettingUIDataValidInfo & validInfo)
{
    //检测On,Off Code数据是否正确
    if (1 == configParser_GetConfigInt(kszBroadsoftActive))
    {
        return true;
    }

    if (!IsPageOnOffCodeValid(stData.m_stCodeInfoAnonymousCall.m_szCodeOn,
                              stData.m_stCodeInfoAnonymousCall.m_szCodeOff))
    {
        validInfo.SetValidIndex(ITEM_CALL_OFFCODE_ID);
        validInfo.SetValidNote(_LANG(TRID_OFF_CODE_CANNOT_BE_EMPTY));
        return false;
    }

    if (!IsPageOnOffCodeValid(stData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn,
                              stData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff))
    {
        validInfo.SetValidIndex(ITEM_REJECTION_OFFCODE_ID);
        validInfo.SetValidNote(_LANG(TRID_OFF_CODE_CANNOT_BE_EMPTY));
        return false;
    }

    return true;
}
