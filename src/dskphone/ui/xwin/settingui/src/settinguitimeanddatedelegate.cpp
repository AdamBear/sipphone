#include "settinguitimeanddatedelegate.h"
#include "settinguiradioitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/dateandtimecontroller.h"

CSettingUITimeAndDateDelegate::CSettingUITimeAndDateDelegate()
{
}

CSettingUITimeAndDateDelegate::~CSettingUITimeAndDateDelegate()
{
}

CSettingUIDelegateBase * CSettingUITimeAndDateDelegate::CreateTimeAndDateDelegate()
{
    CSettingUITimeAndDateDelegate * pDelegate = new CSettingUITimeAndDateDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSettingUITimeAndDateDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    yl::string strCheckID = "";
    int nTimeType = g_pDateAndTimeController.GetTimeType();
    if (DATE_AND_TIME_MANUAL == nTimeType)
    {
        strCheckID = BASIC_MANUAL_SETTING;
    }
    else if (DATE_AND_TIME_SNTP == nTimeType)
    {
        strCheckID = BASIC_SNTP_SETTING;
    }

    SetCheck(strCheckID);

    return m_pPageData;
}

bool CSettingUITimeAndDateDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    return true;
}

void CSettingUITimeAndDateDelegate::SetCheck(const yl::string & strItemID)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    for (int i = 0; i < m_pPageData->GetTotalItemNumber(); ++i)
    {
        CSettingUIRadioItemDataPtr pItemData = CSettingUIRadioItemData::GetRadioListDataItem(
                m_pPageData->GetItemByDataIndex(i));
        if (NULL != pItemData)
        {
            if (strItemID == pItemData->GetItemID())
            {
                pItemData->SetCheck(true);
            }
            else
            {
                pItemData->SetCheck(false);
            }
        }
    }
}
