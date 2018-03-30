#if defined (IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_WIFI)
#include "viewmodeuimanager.h"
#include <settinguilogic/include/settingdefine.h>
#include "settinguimanager.h"
#include "translateiddefine.h"
#include "basesettinguidelegate.h"
#include "settinguibtwifidelegate.h"
#include "baseui/include/modbaseui.h"
#include "messagebox/modmessagebox.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/settinguidialogmanager.h"
#include "settinguilogic/src/settinguibasepagedata.h"

#include "viewmodeuimanager/include/uicommon.h"
#include "viewmodeuimanager/include/viewmodectrler.h"
#include "viewmodeuimanager/include/viewmodectrlerhelper.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "messagebox/selectmessagebox.h"

namespace
{
typedef YLVector<OPTION_TYPE> VEC_OPTION_TYPE;
}

IMPLEMENT_GETINSTANCE(CViewModeUIMgr)

CViewModeUIMgr::CViewModeUIMgr()
{
    //bluetooth
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_BT_MENU,               kszPageIDBluetoothMenu);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_BT_SCAN,               kszPageIDBluetoothScan);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_BT_PAIRED,             kszPageIDBluetoothPaired);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_BT_DEV_DETAIL,         kszPageIDBluetoothDevDetail);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_BT_DONGLE_INFO,        kszPageIDBluetoothEdit);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_BT_PIN_INPUT,          kszPageIDBluetoothPassword);

#ifndef _WIN32
#warning
#endif
//  m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_BT_CHANNEL_CONTROLL,   kszPageIDBTChannelControll);

    //wifi
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_WIFI_MENU,             kszPageIDWifiMenu);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_WIFI_SCAN,             kszPageIDWifiScan);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_WIFI_PROFILE,          kszPageIDWifiKnown);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_WIFI_NET_STATUS,       kszPageIDWifiStatus);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_WIFI_NET_DETAIL,       kszPageIDWifiDetail);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_WIFI_NET_EDIT,         kszPageIDWifiEdit);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_WIFI_NET_ADD,          kszPageIDWifiAdd);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_WIFI_PIN_INPUT,        kszPageIDWifiPassword);
    m_mapUIType2PageName.insert(WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD, kszPageIDWifiDetail2Add);
}

CViewModeUIMgr::~CViewModeUIMgr()
{
}

// 更新界面
void CViewModeUIMgr::UpdateUI(bool bResetFocus/* = false*/)
{
#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("[--ModeUIMgr--]UpdateUI bResetFocus:%d", bResetFocus);
#endif

    if (bResetFocus)
    {
        CSettingUIBtWifiDelegate * pTopDelegate = (CSettingUIBtWifiDelegate *)
                g_pSettingUIManager->GetTopDelegate();
        if (pTopDelegate)
        {
            pTopDelegate->InitExtraData((void *)0);
        }
    }

    CDlgSettingUI * pDlg = GetSettingUIDlg();
    if (pDlg)
    {
        pDlg->RefreshUI(true);
    }
}

// 更新title
bool CViewModeUIMgr::UpdateTitle(yl::string strTitle/* = ""*/)
{
#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("[--ModeUIMgr--]UpdateTitle strTitle:%s", strTitle.c_str());
#endif

    //更新界面Title值
    CSettingUIDelegateBasePtr pTopDelegate = g_pSettingUIManager->GetTopDelegate();
    if (pTopDelegate)
    {
        pTopDelegate->GetPageData()->SetPageTitle(strTitle);
    }
    //更新界面Title显示
    CDlgSettingUI * pDlg = GetSettingUIDlg();
    if (pDlg)
    {
        pDlg->RefreshTitle(strTitle);
    }
    return NULL != pDlg;
}

// 更新item
bool CViewModeUIMgr::UpdateListInfo(bool bResetFocus/* = false*/)
{
#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("[--ModeUIMgr--]UpdateListInfo bResetFocus:%d", bResetFocus);
#endif

    if (bResetFocus)
    {
        CSettingUIBtWifiDelegate * pTopDelegate = (CSettingUIBtWifiDelegate *)
                g_pSettingUIManager->GetTopDelegate();
        if (pTopDelegate)
        {
            pTopDelegate->InitExtraData((void *)0);
        }
    }

    CDlgSettingUI * pDlg = GetSettingUIDlg();
    if (pDlg)
    {
        pDlg->RefreshUI(true);
    }
    return NULL != pDlg;
}

// 更新softkey
bool CViewModeUIMgr::UpdateSoftKet(int iUIType/* = VIEW_MODE_UI_TYPE_BEGIN*/)
{
#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("[--ModeUIMgr--]UpdateSoftKet iUIType:%d", iUIType);
#endif

    CDlgSettingUI * pDlg = GetSettingUIDlg();
    if (pDlg)
    {
        pDlg->RefreshSoftkey();
    }
    return NULL != pDlg;
}

// 更新动画
bool CViewModeUIMgr::UpdateAnimation(bool bEnable)
{
#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("[--ModeUIMgr--]UpdateAnimation bEnable:%d", bEnable);
#endif
    CSettingUIDelegateBase * pTopDelegate = g_pSettingUIManager->GetTopDelegate();

    if (NULL == pTopDelegate)
    {
        return false;
    }

    const yl::string & strPageID = pTopDelegate->GetPageID();

    if (kszPageIDBluetoothScan == strPageID)
    {
        CSettingUIBtWifiDelegate * pBtWifiDelegate = static_cast<CSettingUIBtWifiDelegate *>(pTopDelegate);

        if (NULL != pBtWifiDelegate)
        {
            pBtWifiDelegate->ShowConnectAni(bEnable);
        }
    }

    return true;
}

// 弹出消息框
bool CViewModeUIMgr::PopupMsgBox(int iID, yl::string strTitle,
                                 yl::string strContent, int iStyle, int iTimeOut,
                                 bool bCountDown/* = false*/, bool bQuitEnable/* = true*/)
{
#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("[--ModeUIMgr--]PopupMsgBox iID:%d strTitle:%s strContent:%s iStyle:%d iTimeOut:%d bCountDown:%d bQuitEnable:%d",
                   iID, strTitle.c_str(), strContent.c_str(), iStyle, iTimeOut, bCountDown, bQuitEnable);
#endif

    if (NULL == g_pSettingUIManager
            || (!g_pSettingUIManager->IsPageDelegateCreate(kszPageIDBluetoothMenu)
                && !g_pSettingUIManager->IsPageDelegateCreate(kszPageIDWifiMenu)))
    {
        return false;
    }

    CSettingUIBtWifiDelegate* pTopDelegate = (CSettingUIBtWifiDelegate*)
            g_pSettingUIManager->GetTopDelegate();
    if (NULL == pTopDelegate)
    {
        return false;
    }

    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = (MESSAGEBOX_TYPE)TransformToMyMsgBoxStyle((MsgBoxStyle)iStyle);
    stMsgInfo.strTitle = strTitle;
    stMsgInfo.strNote = strContent;
    stMsgInfo.nMsTime = iTimeOut;
    stMsgInfo.pAction = this;
    stMsgInfo.bCountDown = bCountDown;
    stMsgInfo.iID = iID;
    stMsgInfo.extraData = (void*)(pTopDelegate->GetPageID().c_str());
    MessageBox_ShowCommonMsgBox(stMsgInfo);

    return true;
}

// 移除消息框
void CViewModeUIMgr::RemoveMsgBox(int iID)
{
#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("[--ModeUIMgr--]RemoveMsgBox iID:%d", iID);
#endif

    MessageBox_RemoveMessageBox(this, MessageBox_Close, iID);
}

void CViewModeUIMgr::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    //先处理OptionMsgBox
    if (MSGBOX_ID_OPTION == pMessageBox->GetID() && MESSAGEBOX_SELECTBOX == pMessageBox->GetType())
    {
        CSelectMessageBox * pOptionMsgBox = (CSelectMessageBox *)pMessageBox;
        if (NULL != pOptionMsgBox)
        {
            OptionMsgBoxCallBack(pOptionMsgBox);
        }
        return;
    }

    void* pExtraData = pMessageBox->GetExtraData();
    if (NULL == pExtraData)
    {
        return;
    }

    yl::string strID = (char*)pExtraData;
    CSettingUIBtWifiDelegate* pTopDelegate = (CSettingUIBtWifiDelegate*)
            g_pSettingUIManager->GetTopDelegate();
    if (NULL == pTopDelegate || pTopDelegate->GetPageID() != strID)
    {
        return;
    }

    //后处理普通其他类型MsgBox

    CViewModeControler* pControler = _ViewModeCtrlerHelper.GetCtrler(GetTopUIType());
    if (NULL == pControler)
    {
        return;
    }

#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("OnMsgBoxEvent:iID:%d iKey:%d", pMessageBox->GetID(), pMessageBox->GetResult());
#endif

    pControler->OnMsgBoxEvent(pMessageBox->GetID(), TransformToMsgBoxResult(pMessageBox->GetResult()));

}

void CViewModeUIMgr::OptionMsgBoxCallBack(CSelectMessageBox * pOptionMsgBox)
{
    if (NULL == pOptionMsgBox || MSGBOX_ID_OPTION != pOptionMsgBox->GetID()
            && MESSAGEBOX_SELECTBOX != pOptionMsgBox->GetType())
    {
        return;
    }

    CSettingUIBtWifiDelegate * pTopDelegate = (CSettingUIBtWifiDelegate *)
            g_pSettingUIManager->GetTopDelegate();
    if (NULL == pTopDelegate)
    {
        return;
    }

    CViewModeControler * pControler = _ViewModeCtrlerHelper.GetCtrler(GetTopUIType());
    if (NULL == pControler)
    {
        return;
    }

    int iDataID = DATA_ID_INVALID;
    yl::string strValue;
    if (pTopDelegate->GetEventParam(pTopDelegate->GetFocusIndex(), iDataID, strValue))
    {
        int iActionKey = pOptionMsgBox->GetFcousAction();
        if (MessageBox_OK == pOptionMsgBox->GetResult() && -1 != iActionKey && ST_EMPTY != iActionKey)
        {
#if IF_SETTING_BT_WIFI_DEBUG
            SETTINGUI_INFO("OnOptionEvent iOptionKey:%d :iDataID:%d strValue:%s", iActionKey, iDataID,
                           strValue.c_str());
#endif

            pControler->OnOptionEvent(iActionKey, iDataID, strValue);
        }
    }
}

// 进入页面
bool CViewModeUIMgr::EnterPage(int iUIType, int iFocusIndex/* = 0*/)
{
#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("[--ModeUIMgr--]EnterPage iUIType:%d iFocusIndex:%d", iUIType, iFocusIndex);
#endif

    CSettingUIDelegateBase * pTopDelegate = g_pSettingUIManager->GetTopDelegate();
    if (NULL != pTopDelegate)
    {
        pTopDelegate->SetRefreshDataWhenTopDelegate(true);
    }

    PageOpenData openPageData(GetPageName(iUIType));
    openPageData.pExtraData = (void *)iFocusIndex;
    return g_pSettingUIDialogManager->OpenPage(openPageData);
}

// 退出页面
bool CViewModeUIMgr::ExitPage()
{
#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("[--ModeUIMgr--]ExitPage");
#endif

    CSettingUIDelegateBasePtr pTopDelegate = g_pSettingUIManager->GetTopDelegate();
    if (pTopDelegate)
    {
        pTopDelegate->OnBackAction();
    }

    return NULL != GetSettingUIDlg();
}

// 获取焦点数据ID
int CViewModeUIMgr::GetFocusDataID()
{
    CSettingUIBtWifiDelegate * pTopDelegate = (CSettingUIBtWifiDelegate *)
            g_pSettingUIManager->GetTopDelegate();
    return pTopDelegate ? pTopDelegate->GetFocusDataID() : DATA_ID_INVALID;
}

// 获取顶层页面类型
int CViewModeUIMgr::GetTopUIType()
{
    CSettingUIBtWifiDelegate * pTopDelegate = (CSettingUIBtWifiDelegate *)
            g_pSettingUIManager->GetTopDelegate();
    return pTopDelegate ? pTopDelegate->GetUIType() : VIEW_MODE_UI_TYPE_BEGIN;
}

// 显示option
bool CViewModeUIMgr::ShowOption(bool bShow /*= true*/)
{
#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("[--ModeUIMgr--]ShowOption bShow:%d", bShow);
#endif

    if (!bShow)//关闭Option界面
    {
        MessageBox_RemoveMessageBox(&_ViewModeUIMgr, MessageBox_Close, MSGBOX_ID_OPTION);
        return false;
    }

    CSettingUIBtWifiDelegate * pTopDelegate = (CSettingUIBtWifiDelegate *)
            g_pSettingUIManager->GetTopDelegate();
    if (NULL == pTopDelegate)
    {
        return false;
    }

    CViewModeControler * pControler = _ViewModeCtrlerHelper.GetCtrler(GetTopUIType());
    if (NULL == pControler)
    {
        return false;
    }

    VEC_OPTION_TYPE vecOptionKey;
    if (!pControler->GetOptionSoftkeyList(GetTopUIType(), pTopDelegate->GetFocusIndex(), vecOptionKey))
    {
        return false;
    }

    if (vecOptionKey.size() <= 0)
    {
        return false;
    }

    VEC_OPTION_ITEM_DATA vecOption;
    for (int iIndex = 0; iIndex < vecOptionKey.size(); ++iIndex)
    {
        ST_OPTION_ITEM_DATA optionItemData;
        optionItemData.m_nAction = vecOptionKey[iIndex];
        optionItemData.m_strTitle = _ViewModeUIMgr.GetOptioTitleByOptionType(optionItemData.m_nAction);
        vecOption.push_back(optionItemData);
    }
    BaseUI_ShowOptionMenu(&_ViewModeUIMgr, vecOption, MSGBOX_ID_OPTION);

    return true;
}

// 通过页面类型获取页面名称
yl::string CViewModeUIMgr::GetPageName(int iUIType)
{
    Iter_UIType_2_PageName iter = m_mapUIType2PageName.find(iUIType);
    return iter != m_mapUIType2PageName.end() ? iter->second : "";
}

// 通过页面名称获取页面类型
int CViewModeUIMgr::GetUITypeByPagenName(const yl::string & strPageName)
{
    for (Iter_UIType_2_PageName iter = m_mapUIType2PageName.begin();
            iter != m_mapUIType2PageName.end();
            ++ iter)
    {
        yl::string & strName = iter->second;
        if (strName == strPageName)
        {
            return iter->first;
        }
    }

    return VIEW_MODE_UI_TYPE_BEGIN;
}

bool CViewModeUIMgr::SetDataListFocus(int iDataID)
{
#if IF_SETTING_BT_WIFI_DEBUG
    SETTINGUI_INFO("[--ModeUIMgr--]SetDataListFocus iDataID:%d", iDataID);
#endif

    CSettingUIDelegateBase * pTopDelegate = g_pSettingUIManager->GetTopDelegate();
    CSettingUIBtWifiDelegate * pBtWifiDelegate = static_cast<CSettingUIBtWifiDelegate *>(pTopDelegate);
    if (NULL == pBtWifiDelegate)
    {
        return false;
    }

    int nIndex = pBtWifiDelegate->GetItemIndexByDataID(iDataID);
    pBtWifiDelegate->SetFocusIndex(nIndex);

    CDlgSettingUI * pDlg = GetSettingUIDlg();
    if (NULL != pDlg)
    {
        if (pDlg == UIManager_GetTopWindow() && MessageBox_GetMessageBoxCount() <= 0)
        {
            pDlg->RefreshUI(false);
        }
        else
        {
            pBtWifiDelegate->SetRefreshWndWhileActive(true);
        }
    }

    return true;
}

CDlgSettingUI * CViewModeUIMgr::GetSettingUIDlg()
{
    return CDlgSettingUI::GetDlgBaseSettingUI(g_pSettingUIDialogManager->GetTopActiveDialog());
}

yl::string CViewModeUIMgr::GetSoftKeyTitleByType(int iType)
{
    yl::string strTitle;
    switch (iType)
    {
    case ST_EMPTY:
        strTitle = TRID_EMPTY;
        break;
    case ST_OK:
        strTitle = TRID_OK;
        break;
    case ST_ENTER:
        strTitle = TRID_ENTER;
        break;
    case ST_SAVE:
        strTitle = TRID_SAVE;
        break;
    case ST_BACK:
        strTitle = TRID_BACK;
        break;
    case ST_CANCEL:
        strTitle = TRID_CANCEL;
        break;
    case ST_SCAN:
        strTitle = TRID_SCAN;
        break;
    case ST_REFRESH:
        strTitle = TRID_REFRESH;
        break;
    case ST_SWITCH:
        strTitle = TRID_SWITCH;
        break;
    case ST_OPTION:
        strTitle = TRID_OPTION;
        break;
    case ST_CONNECT:
        strTitle = TRID_CONNECT;
        break;
    case ST_DISCONNECT:
        strTitle = TRID_DISCONNECT;
        break;
    case ST_DETAIL:
        strTitle = TRID_DETAIL;
        break;
    case ST_ADD:
        strTitle = TRID_ADD;
        break;
    case ST_DELETE:
        strTitle = TRID_DELETE;
        break;
    case ST_DELETEALL:
        strTitle = TRID_DELETE_ALL;
        break;
    case ST_WPS:
        strTitle = TRID_WPS;
        break;
    default:
        break;
    }
    return strTitle;
}

yl::string CViewModeUIMgr::GetOptioTitleByOptionType(int eOptionType)
{
    yl::string strTitle = "";
    switch (eOptionType)
    {
    case OPTION_DETAIL:
        strTitle = TRID_DETAIL;
        break;
    case OPTION_DELETE:
        strTitle = TRID_DELETE;
        break;
    case OPTION_DELETE_ALL:
        strTitle = TRID_DELETE_ALL;
        break;
    case OPTION_EDIT:
        strTitle = TRID_EDIT;
        break;
    case OPTION_MOVE_UP:
        strTitle = TRID_MOVE_UP;
        break;
    case OPTION_MOVE_DOWN:
        strTitle = TRID_MOVE_DOWN;
        break;
    case OPTION_WPS:
        strTitle = TRID_WPS;
        break;
    case OPTION_WPS_PIN:
        strTitle = TRID_WPS_PIN;
        break;
    default:
        break;
    }
    return _LANG(strTitle);
}

int CViewModeUIMgr::TransformToMyMsgBoxStyle(int eStyle)
{
    int iStyle = MESSAGEBOX_Error;

    switch (eStyle)
    {
    case MSGBOX_STYLE_OK:
        iStyle = MESSAGEBOX_NOTE;
        break;
    case MSGBOX_STYLE_CANCEL:
        iStyle = MESSAGEBOX_Cancel;
        break;
    case MSGBOX_STYLE_OK_CANCEL:
        iStyle = MESSAGEBOX_OkCancel;
        break;
    default:
        break;
    }

    return iStyle;
}

int CViewModeUIMgr::TransformToMsgBoxResult(int iResult)
{
    MsgBoxResult eResult = MSGBOX_RESULT_NONE;

    switch (iResult)
    {
    case MessageBox_OK:
        eResult = MSGBOX_RESULT_OK;
        break;
    case MessageBox_TimeOut:
        eResult = MSGBOX_RESULT_TIMER_EXIT;
        break;
    case MessageBox_Destory:
    case MessageBox_Close:
        eResult = MSGBOX_RESULT_EXTERNAL_EXIT;
        break;
    case MessageBox_Cancel:
        eResult = MSGBOX_RESULT_CANCEL;
        break;
    default:
        break;
    }

    return eResult;
}
#endif // IF_SUPPORT_BLUETOOTH || IF_SUPPORT_WIFI
