#include "settinguiwifieditdelegate.h"

#ifdef IF_SUPPORT_WIFI

#include "uicallbackfun.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "viewmodeuimanager/include/viewmodeuidefine.h"
#include "viewmodeuimanager/include/viewmodectrlerhelper.h"


CSettingUIWifiEditDelegate::CSettingUIWifiEditDelegate()
{

}

CSettingUIWifiEditDelegate::~CSettingUIWifiEditDelegate()
{

}

CSettingUIDelegateBase * CSettingUIWifiEditDelegate::CreateWifiEditDelegate()
{
    CSettingUIWifiEditDelegate * pDelegate = new CSettingUIWifiEditDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CSettingUIWifiEditDelegate::OnComboBoxValueChange(int nIndex)
{
    if (0 != nIndex || NULL == m_pPageData)
    {
        return;
    }

    CViewModeControler * pControler = _ViewModeCtrlerHelper.GetCtrler(m_iUIType);
    if (NULL == pControler)
    {
        return;
    }

    VecItemDataID2Info vecItemDataID2Info;
    GetVecItemDataID2Info(vecItemDataID2Info);
    VecViewInfo vecInfo;

    // 校正数据
    pControler->CorrectViewInfo(m_iUIType, vecItemDataID2Info, vecInfo);

    if (1 != vecInfo.size())
    {
        return;
    }

    CSettingUIItemDataBase * pItemData = m_pPageData->GetItemByDataIndex(3);
    if (NULL == pItemData)
    {
        return;
    }

    CSettingUIComboBoxItemDataPtr pCmbItemData = CSettingUIComboBoxItemData::GetComboBoxDataItem(
                pItemData);
    if (NULL == pCmbItemData)
    {
        return;
    }

    const ViewInfo & viewInfo = vecInfo[0];

    if (viewInfo.strVecExtraValue.size() != viewInfo.strVecCustomValue.size())
    {
        return;
    }

    pCmbItemData->SetValue(viewInfo.strValue);

    pCmbItemData->ClearOptionItems();

    for (int iIndex = 0; iIndex < viewInfo.strVecExtraValue.size(); ++iIndex)
    {
        CSettingUIComboBoxOption * pCmbOption = new CSettingUIComboBoxOption;
        if (NULL != pCmbOption)
        {
            pCmbOption->m_strDisplayName = viewInfo.strVecExtraValue[iIndex];
            pCmbOption->m_strItemVale = viewInfo.strVecCustomValue[iIndex];
            if (!pCmbItemData->AddOptionItem(pCmbOption))
            {
                delete pCmbOption;
                continue;
            }

            if (atoi(viewInfo.strValue.c_str()) == iIndex)
            {
                pCmbItemData->SetValue(viewInfo.strVecCustomValue[iIndex]);
            }
        }
    }

    g_SetUICallBackFun.RefreshUI(false, m_strPageID);
}

#endif
