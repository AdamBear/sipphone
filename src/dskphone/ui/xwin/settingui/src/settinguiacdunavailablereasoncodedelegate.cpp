#include "settinguiacdunavailablereasoncodedelegate.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguimenudetailitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"


CSettinguiACDUnavailableReasonCodeDelegate::CSettinguiACDUnavailableReasonCodeDelegate()
{

}

CSettinguiACDUnavailableReasonCodeDelegate::~CSettinguiACDUnavailableReasonCodeDelegate()
{

}

CSettingUIDelegateBase *
CSettinguiACDUnavailableReasonCodeDelegate::CreateACDUnavailableReasonCodeDelegate()
{
    CSettinguiACDUnavailableReasonCodeDelegate * pDelegate = new
    CSettinguiACDUnavailableReasonCodeDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSettinguiACDUnavailableReasonCodeDelegate::LoadPageData(
    bool bReLoad /*= false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        LoadListCodeItem(m_pPageData);
    }

    return m_pPageData;
}

bool CSettinguiACDUnavailableReasonCodeDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    int nFocusIndex = pPageData->GetFocusIndex();
    CSettingUIItemDataBasePtr pItem = pPageData->GetItemByDataIndex(nFocusIndex);
    if (NULL == pItem)
    {
        return false;
    }

    yl::string strUnavailReasonCode = pItem->GetDisplayName();

    //如果焦点控件不为输入框，需要解析出code,如Teatime(1):1001
    if (NULL == CSettingUILineEditItemData::GetLineEditDataItem(pItem))
    {
        yl::string::size_type nPos = strUnavailReasonCode.find_last_of(':');
        if (nPos != yl::string::npos)
        {
            strUnavailReasonCode.erase(0, nPos + 1);
        }
    }
    else
    {
        strUnavailReasonCode = pItem->GetValue();
    }

    //设置Unavailable Reason Code
    ACD_SetCurReason(strUnavailReasonCode);
    ACD_ReqChangeACDStatus(AA_UNAVAIL);

    g_SetUICallBackFun.FallBackToPrevousPage();
    g_SetUICallBackFun.RefreshUI(true);

    return true;
}

void CSettinguiACDUnavailableReasonCodeDelegate::LoadListCodeItem(CSettingUIPageDataBase *
        pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    // 获取Unavailable Reason code列表
    DispCodeList listData;
    ACD_GetReasonMap(listData);

    //编号
    int nIndex = 0;
    yl::string strID = "";
    yl::string strDisplay = "";

    //遍历Unavailable Reason code列表,增加到页面上
    for (DispCodeList::iterator it = listData.begin(); it != listData.end(); ++it)
    {
        //设置控件ID
        strID = commonAPI_FormatString("%d", nIndex++);

        const DispCode & codePair = *it;

        //设置控件显示内容
        if (codePair.m_strCodeName.empty())
        {
            strDisplay = codePair.m_strCode;
        }
        else
        {
            strDisplay = commonAPI_FormatString("%s:%s", codePair.m_strCodeName.c_str(),
                                                codePair.m_strCode.c_str());
        }

        CSettingUIDetailMenuItemDataPtr pItemData = new CSettingUIDetailMenuItemData;
        if (NULL != pItemData)
        {
            pItemData->SetItemID(strID);
            pItemData->SetDisplayName(strDisplay);
            pItemData->SetShowIndex(true);
            pItemData->SetShowColon(false);

            if (!pPageData->AddItem(pItemData))
            {
                delete pItemData;
            }
        }
    }
}

#endif
