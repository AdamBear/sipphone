#include "mtswacdmystatusdelegate.h"

#if IF_FEATURE_METASWITCH_ACD

#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguiradioitemdata.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

#include "account/include/modaccount.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"


namespace
{
#define ACDLIST_NOTE_MSGBOX_SHOW_TIME       1500
#define ACD_STATUS_INDEX_FIRST              0
#define ACD_STATUS_INDEX_SECOND             1
}

CMtswAcdMyStatusDelegate::CMtswAcdMyStatusDelegate()
{
}

CMtswAcdMyStatusDelegate::~CMtswAcdMyStatusDelegate()
{
}

CSettingUIDelegateBase* CMtswAcdMyStatusDelegate::CreatMtswAcdMyStatusDelegate()
{
    CMtswAcdMyStatusDelegate* pDelegate = new CMtswAcdMyStatusDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CMtswAcdMyStatusDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() < MIN_SETTING_SOFTKEY_NUM)
    {
        return;
    }

    vecSoftKey[3].m_iType = ST_OK;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_OK);
}

bool CMtswAcdMyStatusDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_OK == iType)
    {
        OnSetAcdStatus();
        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

bool CMtswAcdMyStatusDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (bPress && PHONE_KEY_OK == nKeyCode)
    {
        OnSetAcdStatus();
        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(nKeyCode, bPress, nDataIndex);
}

void CMtswAcdMyStatusDelegate::LoadMTSWStatus()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    AddRadioListItem(_LANG(TRID_AVAILABLE), TRID_AVAILABLE);

    // wrap up
    if (AS_WRAPUP == ACD_GetAcdStatus())
    {
        yl::string strWrapup = MTSWACD_GetLastAnseredMHLG();
        yl::string strStatus;

        //如果有code就显示Code，否则显示Unavailable
        if (strWrapup.empty())
        {
            strStatus = _LANG(TRID_WRAP_UP);
        }
        else
        {
            strStatus = commonAPI_FormatString("%s(%s)", _LANG(TRID_WRAP_UP).c_str(), strWrapup.c_str());
        }

        AddRadioListItem(strStatus, TRID_WRAP_UP);
    }

    LIST_STATUS listStatus;
    MTSWACD_GetUnavailReasonList(listStatus);

    ACD_STATUS eStatus = ACD_GetAcdStatus();
    int nIndex = ACD_STATUS_INDEX_FIRST;
    yl::string strCurUnavailReason;

    if (AS_AVAILABLE == eStatus)
    {
        nIndex = ACD_STATUS_INDEX_FIRST;
    }
    else if (AS_WRAPUP == eStatus)
    {
        nIndex = ACD_STATUS_INDEX_SECOND;
    }
    else
    {
        strCurUnavailReason = MTSWACD_GetCurUnavailReason();
    }

    LIST_STATUS::iterator itr = listStatus.begin();
    LIST_STATUS::iterator itrEnd = listStatus.end();
    int i = nIndex + 1;
    for (; itr != itrEnd; itr++, ++i)
    {
        const yl::string& strStatus = *itr;
        if (!strStatus.empty())
        {
            AddRadioListItem(strStatus, strStatus);
            if (strStatus == strCurUnavailReason)
            {
                nIndex = i;
            }
        }
    }

    // 获取配置，设置选中项，让其显示已选标记，默认选第1项
    CSettingUIRadioItemDataPtr pChooseItem = CSettingUIRadioItemData::GetRadioListDataItem(
                m_pPageData->GetItemByDataIndex(nIndex));
    if (NULL == pChooseItem)
    {
        nIndex = ACD_STATUS_INDEX_FIRST;
        pChooseItem = CSettingUIRadioItemData::GetRadioListDataItem(m_pPageData->GetItemByDataIndex(
                          nIndex));
    }

    if (NULL != pChooseItem)
    {
        pChooseItem->SetCheck(true);
        // 设置默认选中项
        m_pPageData->SetFocusIndex(nIndex);
    }
}

void CMtswAcdMyStatusDelegate::AddRadioListItem(const yl::string& strName, const yl::string& strValue)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIRadioItemDataPtr pItem = new CSettingUIRadioItemData;
    if (NULL != pItem)
    {
        pItem->SetCheck(false);
        pItem->SetShowIndex(true);
        pItem->SetDisplayName(strName);
        pItem->SetValue(strValue);
        if (!m_pPageData->AddItem(pItem))
        {
            delete pItem;
        }
    }
}

void CMtswAcdMyStatusDelegate::OnSetAcdStatus()
{
    // 呼出列表成員
    CSettingUIItemDataBase* pBaseItemData = GetItemDataByDataIndex(GetFocusIndex());
    if (NULL == pBaseItemData)
    {
        return;
    }

    yl::string strValue = pBaseItemData->GetValue();
    ACD_STATUS eStatus = ACD_GetAcdStatus();
    if (strValue.empty() || TRID_WRAP_UP == strValue)
    {
        return;
    }

    if (TRID_AVAILABLE == strValue)
    {
        if (eStatus != AS_AVAILABLE)
        {
            ACD_ReqChangeACDStatus(AA_AVAIL);

            if (AS_WRAPUP == eStatus)
            {
                //进入详情界面
                SettingUI_EnterPage(kszPageMTSWAcdSelectDispcode);
            }
            else
            {
                g_SetUICallBackFun.FallBackToPrevousPage();
            }
        }
    }
    else
    {
        yl::string strUnavailReason;
        ACD_GetUnavailCode(strUnavailReason);
        if (strUnavailReason == strValue)
        {
            return;
        }

        ACD_SetCurReason(strValue);
        ACD_ReqChangeACDStatus(AA_UNAVAIL);

        if (AS_WRAPUP == eStatus)
        {
            //进入详情界面
            SettingUI_EnterPage(kszPageMTSWAcdSelectDispcode);
        }
        else
        {
            g_SetUICallBackFun.FallBackToPrevousPage();
        }
    }
}

CSettingUIPageDataBase* CMtswAcdMyStatusDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIPageDataBase* pPageData = CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        pPageData->SetPageTitle(_LANG(TRID_MTSW_ACD_STATE));
        LoadMTSWStatus();
    }

    return pPageData;
}

#endif

