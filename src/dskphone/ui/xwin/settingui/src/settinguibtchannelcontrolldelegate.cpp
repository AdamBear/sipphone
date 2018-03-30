#include "settinguibtchannelcontrolldelegate.h"
#ifdef IF_SUPPORT_BLUETOOTH

#include "settinguibtchannelcontrollitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"

CSettingUIBtChannelControllDelegate::CSettingUIBtChannelControllDelegate()
{

}

CSettingUIBtChannelControllDelegate::~CSettingUIBtChannelControllDelegate()
{

}

CSettingUIDelegateBase * CSettingUIBtChannelControllDelegate::CreateBtChannelControllDelegate()
{
    CSettingUIBtChannelControllDelegate * pDelegate = new CSettingUIBtChannelControllDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIItemDataBase * CSettingUIBtChannelControllDelegate::GetItem(int nIndex, int nDataID)
{
    if (NULL != m_pPageData)
    {
        for (int i = 0; i < m_pPageData->GetTotalItemNumber(); ++i)
        {
            CSettingUIItemDataBase * pItemData = m_pPageData->GetItemByDataIndex(i);
            if (NULL != pItemData)
            {
                if (nDataID == atoi(pItemData->GetItemID().c_str()))
                {
                    return pItemData;
                }
            }
        }
    }

    return NULL;
}

CSettingUIItemDataBase * CSettingUIBtChannelControllDelegate::CreateDefaultItemData(
    const ViewInfo & viewInfo)
{
    CSettingUIItemDataBase * pItemData = new CSettingUIBTChannelControllItemData;

    return pItemData;
}

bool CSettingUIBtChannelControllDelegate::InitListItemData(CSettingUIItemDataBase * pItemData,
        const ViewInfo & viewInfo)
{
    CSettingUIBTChannelControllItemData * pBTItemData =
        CSettingUIBTChannelControllItemData::GetBTChannelControllDataItem(pItemData);
    if (NULL == pBTItemData)
    {
        return false;
    }

    int nOptionSize = viewInfo.strVecExtraValue.size();
    if (nOptionSize == viewInfo.strVecCustomValue.size() && nOptionSize != 0)
    {
        //combobox对应的数据
        pBTItemData->SetShowIndex(true);
        pBTItemData->SetItemID(commonAPI_FormatString("%d", viewInfo.iDataID));
        pBTItemData->SetDisplayName(viewInfo.strDisplayName);
        pBTItemData->SetValue(viewInfo.strValue);
        pBTItemData->SetExtraData((void *)viewInfo.iDataID);

        pBTItemData->ClearOptionItems();

        for (int iIndex = 0; iIndex < nOptionSize; ++iIndex)
        {
            CSettingUIComboBoxOption * pCmbOption = new CSettingUIComboBoxOption;
            if (NULL != pCmbOption)
            {
                pCmbOption->m_strDisplayName = viewInfo.strVecExtraValue[iIndex];
                pCmbOption->m_strItemVale = viewInfo.strVecCustomValue[iIndex];
                if (!pBTItemData->AddOptionItem(pCmbOption))
                {
                    delete pCmbOption;
                    continue;
                }

                if (atoi(viewInfo.strValue.c_str()) == iIndex)
                {
                    pBTItemData->SetValue(viewInfo.strVecCustomValue[iIndex]);
                }
            }
        }
    }
    else
    {
        //detail对应的数据
        pBTItemData->SetExtraDispname(viewInfo.strDisplayName);
        pBTItemData->SetExtraValue(viewInfo.strValue);
    }

    return true;
}

#endif
