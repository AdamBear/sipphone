#include "settinguibtwifidelegate.h"

#if defined (IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_WIFI)

#include "keymap.h"
#include <settinguilogic/include/settingdefine.h>
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settinguiadapter.h"
#include "viewmodeuimanager.h"
#include "xwindow/xControls.h"
#include "baseui/include/t2xpicpath.h"
#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguimenuitemdata.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguibtchannelcontrollitemdata.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/settinguidialogmanager.h"
#include "settingui/src/settinguimenudetailitemdata.h"
#include "viewmodeuimanager/include/viewmodeuidefine.h"
#include "viewmodeuimanager/include/viewmodectrlerhelper.h"
#include "dskphone/logic/wireless/wifi/include/wifidefines.h"


namespace
{
#if IS_COLOR
#define ANI_PIC_NAME "Theme#animation@bluetooth_rotatecircle"
#define TITLE_ANI_CTRL_WIDHT 24
#define TITLE_ANI_CTRL_HEIGHT 24
#define TITLE_ANI_CTRL_PADDING_X 4
#define TITLE_ANI_CTRL_PIC_NUM 8
#define TITLE_ANI_CTRL_ANI_INTERVAL 125
#else
#define ANI_PIC_NAME "net_connecting_ani"
#define TITLE_ANI_CTRL_WIDHT 19
#define TITLE_ANI_CTRL_HEIGHT 11
#define TITLE_ANI_CTRL_PADDING_X 4
#define TITLE_ANI_CTRL_PIC_NUM 8
#define TITLE_ANI_CTRL_ANI_INTERVAL 125
#endif
}


CSettingUIBtWifiDelegate::CSettingUIBtWifiDelegate()
    : m_iUIType(VIEW_MODE_UI_TYPE_BEGIN)
    , m_iVecInfoNum(0)
    , m_iNewFocusIndex(-1)
    , m_bDataNone(true)
{

}

CSettingUIBtWifiDelegate::~CSettingUIBtWifiDelegate()
{
    xFrameLayout * pTitle = GetTitle();
    xView * pAniBox = NULL;

    if (NULL != pTitle)
    {
        pAniBox = pTitle->getViewById(ANI_PIC_NAME);
    }

    if (NULL != pAniBox)
    {
        pAniBox->close();
        pAniBox = NULL;
    }
}


bool CSettingUIBtWifiDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    if (NULL != pPagedata)
    {
        CSettingUIItemDataBasePtr pItemData = pPagedata->GetItemByItemId(commonAPI_FormatString("%d", WIFI_ITEM_DATA_ID_EDIT_SECTURE));
        if (NULL != pItemData)
        {
            yl::string strValue = pItemData->GetValue();
            int nType = atoi(strValue.c_str());
            if (SERCURE_NONE == nType || SERCURE_WEP == nType || SERCURE_WPA2_PSK == nType || SERCURE_WPA_PSK == nType)
            {
                SetItemShow(pPagedata, commonAPI_FormatString("%d", WIFI_ITEM_DATA_ID_EDIT_EAP_NAME), false);
            }
        }
    }

    return bResult;
}

CSettingUIDelegateBase * CSettingUIBtWifiDelegate::CreateBtWifiDelegate()
{
    CSettingUIBtWifiDelegate * pDelegate = new CSettingUIBtWifiDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

xFrameLayout * CSettingUIBtWifiDelegate::GetTitle()
{
    CSettingDialogBase * pActiveDialog = g_pSettingUIDialogManager->GetTopActiveDialog();

    if (NULL == pActiveDialog)
    {
        return NULL;
    }

    CDlgSettingUI * pDlg = CDlgSettingUI::GetDlgBaseSettingUI(pActiveDialog);

    if (NULL == pDlg)
    {
        return NULL;
    }

    return static_cast<xFrameLayout *>(pDlg->GetTitleView());
}

int CSettingUIBtWifiDelegate::GetUIType()
{
    return m_iUIType;
}

int CSettingUIBtWifiDelegate::GetFocusDataID()
{
    int iDataID = DATA_ID_INVALID;

    if (NULL == m_pPageData)
    {
        return iDataID;
    }

    int iFocusIndex = GetFocusIndex();
    CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(iFocusIndex);
    if (NULL != pItemData)
    {
        iDataID = (int)(pItemData->GetExtraData());
    }

    return iDataID;
}

void CSettingUIBtWifiDelegate::InitExtraData(void * pData)
{
    m_iNewFocusIndex = (int)pData;

    if (IsFirstBindShow())
    {
        m_iUIType = _ViewModeUIMgr.GetUITypeByPagenName(GetPageID());

        CViewModeControler * pControler = _ViewModeCtrlerHelper.GetCtrler(m_iUIType);
        if (NULL != pControler)
        {
#ifndef _WIN32
#warning
#endif
//          pControler->AfterPageLoad(m_iUIType);;
        }
    }
}

CSettingUIPageDataBase * CSettingUIBtWifiDelegate::LoadPageData(bool bReLoad/* = false*/)
{
    int iLastFocusIndex = GetFocusIndex();

    CSettingUIDelegateBase::LoadPageData(bReLoad);
    if (NULL == m_pPageData)
    {
        return m_pPageData;
    }

    CViewModeControler * pControler = _ViewModeCtrlerHelper.GetCtrler(m_iUIType);
    if (NULL == pControler)
    {
        return m_pPageData;
    }

    if (bReLoad)
    {
        InitPageListData();
        //更新Title
        yl::string strTitle = pControler->GetTitle(m_iUIType);
        m_pPageData->SetPageTitle(strTitle);
    }

    if (m_iNewFocusIndex >= 0)
    {
        iLastFocusIndex = m_iNewFocusIndex;
        m_iNewFocusIndex = -1;
    }

    int nTotal = m_pPageData->GetTotalItemNumber();
    if (iLastFocusIndex >= nTotal && nTotal > 0)
    {
        iLastFocusIndex = nTotal - 1;
    }
    SetFocusIndex(iLastFocusIndex);

    return m_pPageData;
}

void CSettingUIBtWifiDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    CViewModeControler * pControler = _ViewModeCtrlerHelper.GetCtrler(m_iUIType);
    if (NULL == pControler)
    {
        return;
    }

    VEC_INT_DATA vecSoftKeyType;

    pControler->GetSoftkeyList(m_iUIType, GetFocusDataID(), vecSoftKeyType);

    if (4 == vecSoftKeyType.size() && 4 == vecSoftKey.size())
    {
        for (int iItem = 0; iItem < vecSoftKey.size(); ++iItem)
        {
            CSoftKeyBarData & objSoftKey = vecSoftKey[iItem];
            if (WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD == m_iUIType && 2 == iItem)
            {
                objSoftKey.m_iType = ST_EMPTY;
                objSoftKey.m_strSoftkeyTitle = _LANG(_ViewModeUIMgr.GetSoftKeyTitleByType(ST_EMPTY));
            }
            else
            {
                const int iKeyType = vecSoftKeyType[iItem];
                objSoftKey.m_iType = iKeyType;
                objSoftKey.m_strSoftkeyTitle = _LANG(_ViewModeUIMgr.GetSoftKeyTitleByType(iKeyType));
            }
        }
    }
}

bool CSettingUIBtWifiDelegate::OnBackAction()
{
    CViewModeControler * pControler = _ViewModeCtrlerHelper.GetCtrler(m_iUIType);
    if (NULL != pControler)
    {
        pControler->PageExit(m_iUIType);
    }

    CSettingUIDelegateBase::OnBackAction();

    g_SetUICallBackFun.FallBackToPrevousPage();
    return true;
}

void CSettingUIBtWifiDelegate::OnReturnToIdle()
{
    CViewModeControler * pControler = _ViewModeCtrlerHelper.GetCtrler(m_iUIType);
    if (NULL != pControler)
    {
        pControler->PageExit(m_iUIType);
    }
    CSettingUIDelegateBase::OnReturnToIdle();
}

bool CSettingUIBtWifiDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_EMPTY == iType)
    {
        return false;
    }

    CViewModeControler * pControler = _ViewModeCtrlerHelper.GetCtrler(m_iUIType);
    if (NULL == pControler)
    {
        return false;
    }

    int iDataID = DATA_ID_INVALID;
    yl::string strValue;
    if (!GetEventParam(GetFocusIndex(), iDataID, strValue))
    {
        return false;
    }

    VecItemDataID2Info vecItemDataID2Info;
    if (ST_OK == iType || ST_SAVE == iType || ST_ENTER == iType)
    {
        GetVecItemDataID2Info(vecItemDataID2Info);
    }

#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("OnSoftKeyEvent iType:%d, iDataID:%d, strValue:%s", iType, iDataID,
                   strValue.c_str());
#endif

    return pControler->OnSoftKeyEvent(iType, iDataID, strValue, (void *)(&vecItemDataID2Info));
}

bool CSettingUIBtWifiDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    CViewModeControler * pControler = _ViewModeCtrlerHelper.GetCtrler(m_iUIType);
    if (NULL == pControler)
    {
        return false;
    }

    int iDataID = DATA_ID_INVALID;
    yl::string strValue;
    if (!GetEventParam(GetFocusIndex(), iDataID, strValue))
    {
        return false;
    }

    if (PHONE_KEY_UP == nKeyCode || PHONE_KEY_DOWN == nKeyCode || PHONE_KEY_OK == nKeyCode)
    {
        VecItemDataID2Info vecItemDataID2Info;
        if (PHONE_KEY_OK == nKeyCode)
        {
            GetVecItemDataID2Info(vecItemDataID2Info);
        }

#if IF_SETTING_BT_WIFI_DEBUG
        SETTINGUI_INFO("OnFunKeyEvent iType:%d, iDataID:%d, strValue:%s", nKeyCode, iDataID,
                       strValue.c_str());
#endif

        return pControler->OnFunKeyEvent(nKeyCode, iDataID, strValue, (void *)(&vecItemDataID2Info));
    }
    else if (PHONE_KEY_1 <= nKeyCode && nKeyCode <= PHONE_KEY_9)
    {
        if ((nKeyCode - PHONE_KEY_1) != GetFocusIndex())
        {
            return false;
        }

        CSettingUIItemDataBase * pItemData = m_pPageData->GetItemByDataIndex(GetFocusIndex());
        if (pItemData && pItemData->GetItemType() == CSettingUILineEditItemData::GetLineEditDataItemType())
        {
            return false;
        }

#if IF_SETTING_BT_WIFI_DEBUG
        SETTINGUI_INFO("OnDigitKeyEvent iType:%d, iDataID:%d, strValue:%s", nKeyCode, iDataID,
                       strValue.c_str());
#endif

        return pControler->OnDigitKeyEvent(nKeyCode, iDataID, strValue);
    }

    return false;
}

void CSettingUIBtWifiDelegate::SaveFocusIndexItemData()
{
    CDlgSettingUI * pDlg = static_cast<CDlgSettingUI *>
                           (g_pSettingUIDialogManager->GetTopActiveDialog());
    if (NULL == pDlg)
    {
        return;
    }

    CSettingUIDelegateBasePtr pCurDelegate = pDlg->GetBindDelegate();
    if (pCurDelegate != this)
    {
        return;
    }

    CSettingUIAdapter * pCurAdapter = pDlg->GetListViewAdapter();
    if (NULL == pCurAdapter)
    {
        return;
    }

    int iFocusIndex = GetFocusIndex();
    pCurDelegate->SetItemDataValueByDataIndex(iFocusIndex,
            pCurAdapter->GetItemValueByDataIndex(iFocusIndex));
}

bool CSettingUIBtWifiDelegate::GetEventParam(int nDataIndex, int & iDataID, yl::string & strValue)
{
    iDataID = DATA_ID_INVALID;
    strValue.clear();

    if (m_bDataNone)
    {
        iDataID = DATA_ID_NONE;
        return true;
    }

    if (NULL == m_pPageData)
    {
        return false;
    }

    CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(nDataIndex);
    if (NULL == pItemData)
    {
        return false;
    }

    SaveFocusIndexItemData();

    //读取iDataID
    if (NULL != pItemData)
    {
        iDataID = (int)(pItemData->GetExtraData());
    }

    //读取strValue
    yl::string strEntireValue = pItemData->GetValue();
    VEC_STRING_DATA vecStrValue = yl::string_split<yl::string::value_type>(strEntireValue,
                                  STR_VALUE_SPLIT_TOKEN, true);
    if (vecStrValue.size() >= 1)
    {
        strValue = vecStrValue[0];
    }

    return true;
}

void CSettingUIBtWifiDelegate::ShowConnectAni(bool bEnable)
{
    xFrameLayout * pTitle = GetTitle();

    if (NULL == pTitle)
    {
        return;
    }

    xAnimationBox * pAniBox = static_cast<xAnimationBox *>(pTitle->getViewById(ANI_PIC_NAME));

    if (bEnable)
    {
        if (NULL == pAniBox)
        {
            pAniBox = new xAnimationBox;

            if (NULL == pAniBox)
            {
                return;
            }

            chRect rcTitle = pTitle->contentRect();

            pAniBox->m_strId = ANI_PIC_NAME;
            pAniBox->layout_width = TITLE_ANI_CTRL_WIDHT;
            pAniBox->layout_height = TITLE_ANI_CTRL_HEIGHT;
#if IS_COLOR
            pAniBox->layout_marginLeft = rcTitle.Width() - TITLE_ANI_CTRL_WIDHT - 10;
#else
            pAniBox->layout_marginLeft = rcTitle.Width() - TITLE_ANI_CTRL_WIDHT;
#endif
            pAniBox->layout_marginTop = (rcTitle.Height() - TITLE_ANI_CTRL_HEIGHT) >> 1;
            pAniBox->paddingLeft = pAniBox->paddingRight = TITLE_ANI_CTRL_PADDING_X;
            pAniBox->SetInterval(TITLE_ANI_CTRL_ANI_INTERVAL);
            pAniBox->overlap = false;

            LIST_STRING listPic;
#if IS_COLOR
            yl::string strPicName = commonAPI_FormatString("%s.png", pAniBox->m_strId.c_str());
            listPic.push_back(PIC_COMMON_ANIMATION_PATH + strPicName);
#else
            for (int i = 1; i <= TITLE_ANI_CTRL_PIC_NUM; ++i)
            {
                yl::string strPicName = commonAPI_FormatString("%s_%d.dob", pAniBox->m_strId.c_str(), i);

                listPic.push_back(PIC_COMMON_PATH + strPicName);
            }
#endif
            pAniBox->SetAnimation(listPic);

            pTitle->addChildView(pAniBox);
        }

        pAniBox->show();
    }
    else if (NULL != pAniBox)
    {
        pAniBox->hide();
    }
}

bool CSettingUIBtWifiDelegate::GetVecItemDataID2Info(VecItemDataID2Info & vecItemDataID2Info)
{
    vecItemDataID2Info.clear();
    if (WIRELESS_UI_TYPE_BT_MENU         == m_iUIType
            || WIRELESS_UI_TYPE_BT_DONGLE_INFO  == m_iUIType
            || WIRELESS_UI_TYPE_BT_PIN_INPUT    == m_iUIType
            || WIRELESS_UI_TYPE_WIFI_NET_EDIT   == m_iUIType
            || WIRELESS_UI_TYPE_WIFI_PIN_INPUT  == m_iUIType
            || WIRELESS_UI_TYPE_WIFI_NET_ADD    == m_iUIType
            || WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD == m_iUIType)
    {
        //读取vecItemDataID2Info
        int iFocusIndex = GetFocusIndex();
        for (int iDataIndex = 0; iDataIndex < m_iVecInfoNum; ++iDataIndex)
        {
            CSettingUIItemDataBase * pItemData = m_pPageData->GetItemByDataIndex(iDataIndex);
            if (NULL == pItemData)
            {
                continue;
            }
            ItemDataID2Info itemInfo;
            itemInfo.iDataID = (int)(pItemData->GetExtraData());
            itemInfo.bIsFocus = (iFocusIndex == iDataIndex);

            yl::string strEntireValue = pItemData->GetValue();
            VEC_STRING_DATA vecStrValue = yl::string_split<yl::string::value_type>(strEntireValue,
                                          STR_VALUE_SPLIT_TOKEN, true);
            if (vecStrValue.size() >= 1)
            {
                itemInfo.strValue = vecStrValue[0];
                itemInfo.strCustomValue = vecStrValue[0];
            }

#if IF_SETTING_BT_WIFI_DEBUG
            SETTINGUI_INFO("itemDataID2Info iDataID:%d bIsFocus:%d strValue:%s strCustomValue:%s",
                           itemInfo.iDataID, itemInfo.bIsFocus, itemInfo.strValue.c_str(), itemInfo.strCustomValue.c_str());
#endif

            vecItemDataID2Info.push_back(itemInfo);
        }
    }
#ifndef _WIN32
#warning
#endif
    else if (false)//WIRELESS_UI_TYPE_BT_CHANNEL_CONTROLL == m_iUIType)
    {
        if (NULL == m_pPageData)
        {
            return false;
        }

#ifdef IF_SUPPORT_BLUETOOTH
        for (int i = 0; i < m_pPageData->GetTotalItemNumber(); ++i)
        {
            CSettingUIBTChannelControllItemData * pItemData =
                CSettingUIBTChannelControllItemData::GetBTChannelControllDataItem(m_pPageData->GetItemByDataIndex(i));
            if (NULL != pItemData)
            {
                int iDataID = atoi(pItemData->GetItemID().c_str());
                if (BT_ITEM_DATA_ID_DEVICE_MOBILE_ENABLE == iDataID
                        || BT_ITEM_DATA_ID_DEVICE_MUSIC_ENABLE == iDataID)
                {
                    ItemDataID2Info itemInfo;
                    itemInfo.iDataID = iDataID;
                    itemInfo.strValue = pItemData->GetValue();
                    vecItemDataID2Info.push_back(itemInfo);
                }
            }
        }
#endif
    }

    return (vecItemDataID2Info.size() > 0);
}

void CSettingUIBtWifiDelegate::InitPageListData()
{
    m_iVecInfoNum = 0;
    m_bDataNone = true;

    if (NULL == m_pPageData)
    {
        return;
    }

    CViewModeControler * pControler = _ViewModeCtrlerHelper.GetCtrler(m_iUIType);
    if (NULL == pControler)
    {
        return;
    }

    VecViewInfo vecViewInfo;
    pControler->GetDataList(m_iUIType, 0, vecViewInfo);

#if IF_SETTING_BT_WIFI_DEBUG
    printfVecViewInfo(vecViewInfo);
#endif

    if (vecViewInfo.GetSize() < 1)
    {
        return;
    }

    if (1 == vecViewInfo.GetSize() && DATA_ID_NONE == vecViewInfo[0].iDataID)
    {
        m_bDataNone = true;
        return;
    }

    bool bAdd = false;
    CSettingUIItemDataBase * pItemData = NULL;
    int iIndex = 0;
    for (; iIndex < vecViewInfo.GetSize(); ++iIndex)
    {
        bAdd = false;

        const ViewInfo & viewInfo = vecViewInfo[iIndex];

        pItemData = GetItem(iIndex, viewInfo.iDataID);
        if (NULL == pItemData)
        {
            pItemData = CreateDefaultItemData(viewInfo);
            if (NULL == pItemData)
            {
                continue;
            }
            bAdd = true;
        }

        if (!InitListItemData(pItemData, viewInfo))
        {
            continue;
        }

        if (bAdd && !m_pPageData->AddItem(pItemData))
        {
            if (NULL != pItemData)
            {
                delete pItemData;
                pItemData = NULL;
            }
            continue;
        }

        ++m_iVecInfoNum;
        m_bDataNone = false;
    }

    for (; iIndex < m_pPageData->GetTotalItemNumber(); ++iIndex)
    {
        pItemData = m_pPageData->GetItemByDataIndex(iIndex);
        if (NULL != pItemData)
        {
            pItemData->SetResetable(false);
            pItemData->SetShow(false);
        }
    }
}

CSettingUIItemDataBase * CSettingUIBtWifiDelegate::GetItem(int nIndex, int nDataID)
{
    if (NULL != m_pPageData)
    {
        return m_pPageData->GetItemByDataIndex(nIndex);
    }

    return NULL;
}

CSettingUIItemDataBase * CSettingUIBtWifiDelegate::CreateDefaultItemData(const ViewInfo & viewInfo)
{
    CSettingUIItemDataBase * pItemData = NULL;

    if (viewInfo.strVecExtraValue.size() == viewInfo.strVecCustomValue.size()
            && viewInfo.strVecExtraValue.size() > 0)
    {
        pItemData = new CSettingUIComboBoxItemData;
    }
    else if (viewInfo.strValue.empty())
    {
        pItemData = new CSettingUIMenuItemData;
    }
    else
    {
        pItemData = new CSettingUIDetailMenuItemData;
    }

    return pItemData;
}

bool CSettingUIBtWifiDelegate::InitListItemData(CSettingUIItemDataBase * pItemData,
        const ViewInfo & viewInfo)
{
    if (NULL == pItemData)
    {
        return false;
    }

    pItemData->SetItemID(commonAPI_FormatString("%d", viewInfo.iDataID));
    pItemData->SetDisplayName(viewInfo.strDisplayName);
    pItemData->SetValue(viewInfo.strValue);

    pItemData->SetExtraData((void *)viewInfo.iDataID);

    bool bShowIndex = (WIRELESS_UI_TYPE_BT_PIN_INPUT != m_iUIType
                       && WIRELESS_UI_TYPE_WIFI_PIN_INPUT != m_iUIType);
    pItemData->SetShowIndex(bShowIndex);

    if (WIRELESS_UI_TYPE_WIFI_NET_STATUS == m_iUIType
            || WIRELESS_UI_TYPE_WIFI_NET_DETAIL == m_iUIType
            || WIRELESS_UI_TYPE_WIFI_NET_EDIT   == m_iUIType
            || WIRELESS_UI_TYPE_WIFI_NET_ADD    == m_iUIType
            || WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD == m_iUIType)
    {
        pItemData->SetShowColon(false);
    }

    if (CSettingUIComboBoxItemData::GetComboBoxDataItemType() == pItemData->GetItemType())
    {
        CSettingUIComboBoxItemDataPtr pCmbItemData = CSettingUIComboBoxItemData::GetComboBoxDataItem(
                    pItemData);
        if (NULL != pCmbItemData && viewInfo.strVecExtraValue.size() == viewInfo.strVecCustomValue.size())
        {
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
                    }

                    if (atoi(viewInfo.strValue.c_str()) == iIndex)
                    {
                        pCmbItemData->SetValue(viewInfo.strVecCustomValue[iIndex]);
                    }
                }
            }
        }
    }

    return true;
}

int CSettingUIBtWifiDelegate::GetItemIndexByDataID(int nDataID)
{
    if (NULL == m_pPageData)
    {
        return -1;
    }

    for (int i = 0; i < m_pPageData->GetTotalItemNumber(); ++i)
    {
        CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(i);
        if (NULL != pItemData)
        {
            if ((int)(pItemData->GetExtraData()) == nDataID)
            {
                return i;
            }
        }
    }

    return -1;
}

#if IF_SETTING_BT_WIFI_DEBUG
void CSettingUIBtWifiDelegate::printfVecViewInfo(const VecViewInfo & vecViewInfo)
{
    SETTINGUI_INFO("CSettingUIBtWifiDelegate::printfVecViewInfo size:%d", vecViewInfo.GetSize());
    for (int iItem = 0; iItem < vecViewInfo.GetSize(); ++iItem)
    {
        const ViewInfo & viewInfo = vecViewInfo[iItem];
        SETTINGUI_INFO("iTtem:%d", iItem);
        SETTINGUI_INFO("iDataID:%d", viewInfo.iDataID);
        SETTINGUI_INFO("strDisplayName:%s", viewInfo.strDisplayName.c_str());
        SETTINGUI_INFO("strValue:%s", viewInfo.strValue.c_str());
        SETTINGUI_INFO("strValue1:%s", viewInfo.strValue1.c_str());
        SETTINGUI_INFO("bUseValue:%d", viewInfo.bUseValue);
        SETTINGUI_INFO("strVecExtraValue size:%d", viewInfo.strVecExtraValue.size());
        for (int i = 0; i < viewInfo.strVecExtraValue.size(); ++i)
        {
            SETTINGUI_INFO("strVecExtraValue index:%d value:%s", i, viewInfo.strVecExtraValue[i].c_str());
        }
        SETTINGUI_INFO("strVecCustomValue size:%d", viewInfo.strVecCustomValue.size());
        for (int i = 0; i < viewInfo.strVecCustomValue.size(); ++i)
        {
            SETTINGUI_INFO("strVecCustomValue index:%d value:%s", i, viewInfo.strVecCustomValue[i].c_str());
        }
    }
}
#endif

#endif // (IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_WIFI)
