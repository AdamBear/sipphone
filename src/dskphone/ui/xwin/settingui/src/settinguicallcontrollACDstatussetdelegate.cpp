#include "settinguicallcontrollACDstatussetdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "settinguiradioitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define INDEX_AVAILABLE_NORMAL 1
#define INDEX_AVAILABLE_ON_ITEM_HIDE 0
#define INDEX_UNAVAILABLE_NORMAL 2
#define INDEX_UNAVAILABLE_ON_ITEM_HIDE 1
#define INDEX_LOGOUT_NORMAL 4
#define INDEX_LOGOUT_ON_ITEM_HIDE 2
#define INDEX_WRAPUP 3
}

CSettinguiCallControllACDStatusSetDelegate::CSettinguiCallControllACDStatusSetDelegate()
    : m_bHideSomeItem(false)
{

}

CSettinguiCallControllACDStatusSetDelegate::~CSettinguiCallControllACDStatusSetDelegate()
{

}

CSettingUIDelegateBase * CSettinguiCallControllACDStatusSetDelegate::CreateACDStatusSetDelegate()
{
    CSettinguiCallControllACDStatusSetDelegate * pDelegate = new
    CSettinguiCallControllACDStatusSetDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSettinguiCallControllACDStatusSetDelegate::LoadPageData(
    bool bReLoad /*= false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        m_bHideSomeItem = false;
#if IF_FEATURE_START2START_ACD
        if (ACD_START2START == Get_ACDType())
        {
            m_bHideSomeItem = true;
        }
#endif
        m_bHideSomeItem |= ACD_IsGensACD();

        //获取ACD状态
        ACD_STATUS eACDStatus = ACD_GetAcdStatus();
        int nFocusIndex = 0;

        if (AS_AVAILABLE == eACDStatus)
        {
            SetCheckByID(ACDSTATEPAGE_AVAILABLE);
            nFocusIndex = m_bHideSomeItem ? INDEX_AVAILABLE_ON_ITEM_HIDE : INDEX_AVAILABLE_NORMAL;
        }
        else if (AS_UNAVAILABLE == eACDStatus)
        {
            SetCheckByID(ACDSTATEPAGE_UNAVAILABLE);
            nFocusIndex = m_bHideSomeItem ? INDEX_UNAVAILABLE_ON_ITEM_HIDE : INDEX_UNAVAILABLE_NORMAL;
        }
        else if (AS_WRAPUP == eACDStatus)
        {
            SetCheckByID(ACDSTATEPAGE_WRAPUP);
            nFocusIndex = INDEX_WRAPUP;
        }
        else if (AS_IDLE == eACDStatus)
        {
            SetCheckByID(ACDSTATEPAGE_SIGNOUT);
            nFocusIndex = m_bHideSomeItem ? INDEX_LOGOUT_ON_ITEM_HIDE : INDEX_LOGOUT_NORMAL;
        }

        SetFocusIndex(nFocusIndex);
    }

    return m_pPageData;
}

bool CSettinguiCallControllACDStatusSetDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    bool bResult = CSettinguiDataDelegateBase::PageDataFilter(pPagedata, bReload);

    if (NULL != pPagedata && m_bHideSomeItem)
    {
        SetItemShow(pPagedata, ACDSTATEPAGE_WRAPUP, false);
        SetItemShow(pPagedata, ACDSTATEPAGE_SIGNIN, false);
    }

    return bResult;
}

bool CSettinguiCallControllACDStatusSetDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    //获取ACD状态
    ACD_STATUS eAcdState = ACD_GetAcdStatus();
    BOOL bQuitPage = TRUE;

    int nFocusIndex = pPageData->GetFocusIndex();
    yl::string strItemId = GetItemIDByDataIndex(nFocusIndex);
    if (strItemId.empty())
    {
        return false;
    }

    if (ACDSTATEPAGE_SIGNIN == strItemId)
    {
        if (AS_IDLE == eAcdState)
        {
            //如果是处于登出状态，则登录
            bool bLogin = ACD_ReqChangeACDStatus(AA_LOGIN);
            if (!bLogin)
            {
                m_bBackToPreviousInterface = true;
                ShowNoteMessgaeBox(_LANG(TRID_LOGIN_FAILED), -1);
                return false;
            }
        }
    }
    else if (ACDSTATEPAGE_AVAILABLE == strItemId)
    {
        if (AS_AVAILABLE != eAcdState)
        {
            //设置ACD状态为Available
            ACD_ReqChangeACDStatus(AA_AVAIL);
        }
    }
    else if (ACDSTATEPAGE_UNAVAILABLE == strItemId)
    {
        if (AS_UNAVAILABLE != eAcdState)
        {
            if (CallCenter_IsUnavailReasonCodeOn())
            {
                PageOpenData opPageData(kszPageIDACDUnavailableReasonCode);
                g_SetUICallBackFun.EnterPage(opPageData);
                bQuitPage = false;
            }
            else
            {
                //设置ACD为Unavailable
                ACD_ReqChangeACDStatus(AA_UNAVAIL);
            }
        }
    }
    else if (ACDSTATEPAGE_WRAPUP == strItemId)
    {
        if (AS_WRAPUP != eAcdState)
        {
            //设置ACD状态为Wrap-up
            ACD_ReqChangeACDStatus(AA_WRAPUP);
        }
    }
    else if (ACDSTATEPAGE_SIGNOUT == strItemId)
    {
        if (AS_IDLE != eAcdState)
        {
            //登出ACD
            ACD_ReqChangeACDStatus(AA_LOGOUT);
            if (Get_ACDType() == ACD_GENESYS)
            {
                // 手动登出记录到配置文件中
                configParser_SetCfgItemIntValue(kszACDStatus, ACD_GetAccountId(), AS_IDLE, CONFIG_LEVEL_PHONE);
            }
        }
    }

    if (bQuitPage)
    {
        g_SetUICallBackFun.FallBackToPrevousPage();
    }

    return true;
}

void CSettinguiCallControllACDStatusSetDelegate::SetCheckByID(const yl::string & strItemID)
{
    CSettingUIRadioItemDataPtr pItem = CSettingUIRadioItemData::GetRadioListDataItem(GetItemByItemId(
                                           strItemID));
    if (NULL != pItem)
    {
        pItem->SetCheck(true);
    }
}

#endif
