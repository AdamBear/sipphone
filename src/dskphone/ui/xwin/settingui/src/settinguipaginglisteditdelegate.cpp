#include "settinguipaginglisteditdelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/include/common.h"
#include "baseui/include/baseuicommon.h"
#include "feature/include/featurecommon.h"
#include "feature/include/modfeature.h"
#include "translateiddefine.h"
#include "commonunits/modcommonunits.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define ITEM_INDEX_ADDRESS 1
#define ITEM_INDEX_LABEL 2
#define ITEM_INDEX_CHANNEL 3
}

CSettinguiPagingListEditDelegate::CSettinguiPagingListEditDelegate()
{

}

CSettinguiPagingListEditDelegate::~CSettinguiPagingListEditDelegate()
{

}

CSettingUIDelegateBase * CSettinguiPagingListEditDelegate::CreatPagingListEditDelegate()
{
    CSettinguiPagingListEditDelegate * pDelegate = new CSettinguiPagingListEditDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSettinguiPagingListEditDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    const PagingGroupInfo * pPagingGroup = feature_MutliPaingGetGroupInfo(m_iLineID);
    if (NULL != pPagingGroup)
    {
        SetComboboxValue(PAGING_LIST_EDIT_NUMBER, pPagingGroup->iIndex);
        SetValue(PAGING_LIST_EDIT_ADDRESS, pPagingGroup->strAddress);
        SetValue(PAGING_LIST_EDIT_LABEL, pPagingGroup->strLabel);
        SetComboboxValue(PAGING_LIST_EDIT_CHANNEL, pPagingGroup->iChannel);
    }

    return m_pPageData;
}

bool CSettinguiPagingListEditDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return true;
    }

    PagingGroupInfo stPagingGroupInfo;
    stPagingGroupInfo.iIndex = GetComboboxValue(PAGING_LIST_EDIT_NUMBER);
    stPagingGroupInfo.strAddress = GetValue(PAGING_LIST_EDIT_ADDRESS);
    stPagingGroupInfo.strLabel = GetValue(PAGING_LIST_EDIT_LABEL);
    stPagingGroupInfo.iChannel = GetComboboxValue(PAGING_LIST_EDIT_CHANNEL);
    if (-1 == stPagingGroupInfo.iChannel)
    {
        stPagingGroupInfo.iChannel = 0;
    }

    CSettingUIDataValidInfo validInfo;
    if (!IsInputDataValid(stPagingGroupInfo.strAddress, stPagingGroupInfo.strLabel, validInfo))
    {
        g_SetUICallBackFun.DealWithValidData(validInfo, m_strPageID);
        return true;
    }

    if (feature_MutliPaingSaveGroupInfo(stPagingGroupInfo))
    {
        ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE_AND_REFRESH);
    }

    return true;
}

bool CSettinguiPagingListEditDelegate::IsInputDataValid(const yl::string & strAddress,
        const yl::string & strLabel, CSettingUIDataValidInfo & validInfo)
{
    yl::string strChannel = GetValue(PAGING_LIST_EDIT_CHANNEL);
    if (strAddress.empty() && strLabel.empty() && strChannel.empty())
    {
        return true;
    }

    if (strAddress.empty())
    {
        validInfo.SetValidIndex(ITEM_INDEX_ADDRESS);
        validInfo.SetValidNote(_LANG(TRID_INFORMATION_NOT_COMPLETE));
        return false;
    }

    if (strLabel.empty())
    {
        validInfo.SetValidIndex(ITEM_INDEX_LABEL);
        validInfo.SetValidNote(_LANG(TRID_INFORMATION_NOT_COMPLETE));
        return false;
    }

    if (!commonUnits_IsValidPagingChannel(strChannel))
    {
        validInfo.SetValidIndex(ITEM_INDEX_CHANNEL);
        validInfo.SetValidNote(_LANG(TRID_SAVE_CHANNEL_ERROR));
        return false;
    }

    return true;
}
