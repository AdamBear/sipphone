#include "settinguifeaturesdnddelegate.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "baseui/include/baseuicommon.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguidatavalidinfo.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/featurecontroller.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "account/include/account_datadefine.h"
#include "account/include/modaccount.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "feature/include/modfeature.h"

namespace
{
#define ITEM_DND_OFFCODE_ID 2
#define MSGBOX_ID_ALL_LINE 22
}

CSettinguiFeaturesDNDDelegate::CSettinguiFeaturesDNDDelegate()
{

}

CSettinguiFeaturesDNDDelegate::~CSettinguiFeaturesDNDDelegate()
{

}

CSettingUIDelegateBase * CSettinguiFeaturesDNDDelegate::CreateFeatureDNDDelegate()
{
    CSettinguiFeaturesDNDDelegate * pDelegate = new CSettinguiFeaturesDNDDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettinguiFeaturesDNDDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /* = NULL */)
{
    if (SETTING_MessageBox_OK == nResult && MSGBOX_ID_ALL_LINE == nID)
    {
        //获取当前数据
        DndData stDndData;
        stDndData.m_bEnabled = GetComboboxValue(FEATURES_DND_ENABLE);
        stDndData.m_strOnCode = GetValue(FEATURES_DND_ONCODE);
        stDndData.m_strOffCode = GetValue(FEATURES_DND_OFFCODE);
        //拷贝到所有账号
        int iAccountNum = acc_AccountNum();
        for (int i = 0; i < iAccountNum; i++)
        {
            if (acc_IsAccountAvailable(i))
            {
                SetDndData(stDndData, i);
            }
        }

        ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE_AND_REFRESH);

        return true;
    }

    return CSettingUIAccountDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

void CSettinguiFeaturesDNDDelegate::InitExtraData(void * pData)
{
    CSettingUIAccountDelegateBase::InitExtraData(pData);
}

CSettingUIPageDataBase * CSettinguiFeaturesDNDDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    bool bPhone = kszPageIDPhoneDND == GetPageID();

    DndData stDndData;
    if (GetDndData(&stDndData, m_iLineID))
    {
        SetComboboxValue(FEATURES_DND_ENABLE, stDndData.m_bEnabled);
        SetValue(FEATURES_DND_ONCODE, stDndData.m_strOnCode);
        SetValue(FEATURES_DND_OFFCODE, stDndData.m_strOffCode);
    }

    CSettingUIComboBoxItemDataPtr pItemData = static_cast<CSettingUIComboBoxItemData *>(GetItemByItemId(
                FEATURES_DND_ENABLE));
    if (NULL != pItemData)
    {
        pItemData->SetSwitchSoftKey(!bPhone);
    }

    return m_pPageData;
}

bool CSettinguiFeaturesDNDDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIAccountDelegateBase::PageDataFilter(pPagedata, bReload);

    //broadsoft开启之后不显示code
    bool bShow = true;
    if (1 == configParser_GetConfigInt(kszBroadsoftActive)
#if IF_FEATURE_METASWITCH
            || configParser_GetConfigInt(kszMetaSwitchActive) != 0
#endif
#ifdef IF_FEATURE_PSTN
            || (PSTN_LINE_BEGIN_ID <= m_iLineID  && PSTN_LINE_END_ID >= m_iLineID)
#endif
       )
    {
        bShow = false;
    }

    SetItemShow(pPagedata, FEATURES_DND_ONCODE, bShow);
    SetItemShow(pPagedata, FEATURES_DND_OFFCODE, bShow);

    return bResult;
}

bool CSettinguiFeaturesDNDDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == m_pPageData || !pPageData->IsEnable())
    {
        return true;
    }

    DndData stDndData;
    stDndData.m_bEnabled = GetComboboxValue(FEATURES_DND_ENABLE);
    stDndData.m_strOnCode = GetValue(FEATURES_DND_ONCODE);
    stDndData.m_strOffCode = GetValue(FEATURES_DND_OFFCODE);

    if (0 == configParser_GetConfigInt(kszBroadsoftActive)
            && IsItemShow(FEATURES_DND_ONCODE)
            && IsItemShow(FEATURES_DND_OFFCODE)
            && !IsPageOnOffCodeValid(stDndData.m_strOnCode, stDndData.m_strOffCode))
    {
        CSettingUIDataValidInfo validInfo;
        validInfo.SetValidIndex(ITEM_DND_OFFCODE_ID);
        validInfo.SetValidNote(_LANG(TRID_OFF_CODE_CANNOT_BE_EMPTY));
        g_SetUICallBackFun.DealWithValidData(validInfo, m_strPageID);
        return false;
    }

    SetDndData(stDndData, m_iLineID);

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE_AND_REFRESH);

    return true;
}

void CSettinguiFeaturesDNDDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (SOFTKEY_NUM != vecSoftKey.size() && NULL == m_pPageData)
    {
        return;
    }

#if IF_SUPPORT_CUSTOM_DND_CONTROL_ALL_LINES
    if (kszPageIDAccountDND == GetPageID() && m_pPageData->IsEnable())
    {
        vecSoftKey[1].m_iType = ST_ALL_LINES;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ALL_LINES);
    }
    else
    {
        vecSoftKey[1].m_iType = ST_EMPTY;
        vecSoftKey[1].m_strSoftkeyTitle = "";
    }
#endif
}

bool CSettinguiFeaturesDNDDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_ALL_LINES == iType)
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, "",
                                               _LANG(TRID_IF_COPY_CONFIGURATION_TO_ALL_LINES)
                                               , SETTING_MESSAGEBOX_OkCancel, "", "", -1, MSGBOX_ID_ALL_LINE);
        return true;
    }

    return CSettingUIAccountDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}
