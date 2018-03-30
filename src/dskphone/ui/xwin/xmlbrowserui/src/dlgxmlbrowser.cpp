#include "dlgxmlbrowser.h"
#include "settingui/src/uicallbackfun.h"
#include "imeedit/ximeedit.h"
#include "keymap.h"

#include "baseui/include/t2xpicpath.h"
#include "baseui/include/t2xwidget.h"
#include "baseui/include/baseuicommon.h"
#include "xwindow/xSoftkeyBar.h"
#include "autopcontrol/include/modautopcontrol.h"

#include "settinguilogic/include/common.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/messageboxbase.h"

#include "settingui/include/modsettingui.h"
#include "settingui/include/pageiddefine.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "commonunits/modcommonunits.h"
#include "xmlbrowseruiframe.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "voice/include/modvoice.h"
#include "uimanager/modvolumebar.h"
#ifdef IF_OLD_BLUETOOTH
#include "wireless/bluetoothOld/include/modbluetooth.h"
#else
#include "wireless/bluetoothNew/include/modbluetooth.h"
#endif // IF_OLD_BLUETOOTH

#if XML_BROWSER_SUPPORT_UI

namespace
{
#define DLG_XML_BROWSER_XML "xmlbrowserui/dlgxmlbrowser.xml"
#define XMLBROWSER_FRAME    "xmlFrame"
#define RING_MIN_VALUE      0
#define RING_MAX_VALUE      15
}

CBaseDialog * CDlgXMLBrowser::CreateInstance()
{
    CDlgXMLBrowser * pDlg = new CDlgXMLBrowser;

    if (NULL != pDlg)
    {
        pDlg->loadContent(DLG_XML_BROWSER_XML);
    }

    return pDlg;
}

CDlgXMLBrowser::CDlgXMLBrowser()
    : CBaseDialog(this, eWT_XMLBrower)
    , m_pXBUIFrame(NULL)
    , m_bFullScreen(false)
{
    Volumebar_ConnectVisibleChangeSlot(SLOT_METHOD(this, CDlgXMLBrowser::OnVolumeVisibleChanged));
}

CDlgXMLBrowser::~CDlgXMLBrowser()
{
    UIManager_LockReturnToIdle(false);
    Volumebar_DisconnectVisibleChangeSlot(SLOT_METHOD(this, CDlgXMLBrowser::OnVolumeVisibleChanged));
}

void CDlgXMLBrowser::Init()
{

}

void CDlgXMLBrowser::Uninit()
{

}

bool CDlgXMLBrowser::IsShowSoftkey()
{
    return !m_bFullScreen;
}

void CDlgXMLBrowser::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }
}

void CDlgXMLBrowser::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);
    m_pXBUIFrame = static_cast<xXMLBrowserUIFrame *>(getViewById(XMLBROWSER_FRAME));
    InitData();
}

void CDlgXMLBrowser::UpdateSoftkey()
{
    XMLBROWSERUI_INFO("CDlgXMLBrowser::UpdateSoftkey");
    if (NULL != m_pXBUIFrame)
    {
        UpdateSoftkey(m_pXBUIFrame->GetCurrentUISoftkey());
    }
}

// 初始化数据
void CDlgXMLBrowser::InitData()
{
    if (NULL != m_pXBUIFrame)
    {
        // 连接相应的信号和槽
        m_pXBUIFrame->InitData();
    }
}

// 初始化数据
void CDlgXMLBrowser::ExitXmlUI(int nElementID)
{
    if (NULL != m_pXBUIFrame)
    {
        // 连接相应的信号和槽
        m_pXBUIFrame->ExitXmlUI(nElementID);
    }
}

// 处理RegionAction
void CDlgXMLBrowser::OnChangeSoftkey(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                     LPVOID pData)
{
    UpdateSoftkey();
}

// 更新softkey数据
void CDlgXMLBrowser::UpdateSoftkey(VecSoftkeyParam & refSoftkeyData)
{
    XMLBROWSERUI_INFO("CDlgXMLBrowser::UpdateSoftkey");
    // 清空当前softkey数据
    m_SoftkeyData.clear();

    CSoftKeyBarData tempData;
    for (int i = 0; i != refSoftkeyData.size(); ++i)
    {
        const SoftkeyParam & softParam = refSoftkeyData[i];

        // softkey more由UI控件处理
        if (XB_ST_MORE == softParam.eType)
        {
            continue;
        }
        // 设置显示名
        tempData.m_strSoftkeyTitle  = _LANG(softParam.strName.c_str());
        tempData.m_iType = ST_IDLE_HOTKEY1 + i;
        m_SoftkeyData.push_back(tempData);
    }
    SetSoftKey(m_SoftkeyData);
}

bool CDlgXMLBrowser::OnSoftkeyAction(int iType, bool bLongPress)
{
    if (!bLongPress && NULL != m_pXBUIFrame)
    {
        m_pXBUIFrame->OnSoftkeyPress(iType - ST_IDLE_HOTKEY1);
    }
    return false;
}

// 更新界面
void CDlgXMLBrowser::UpdateUI(const XMLLogicToUIData & objData)
{
    XMLBROWSERUI_INFO("CDlgXMLBrowser::UpdateUI");
    if (NULL != m_pXBUIFrame)
    {
        m_pXBUIFrame->UpdateUI(objData);
        UpdateSoftkey();
    }
}

bool CDlgXMLBrowser::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (NULL != m_pXBUIFrame)
    {
        return m_pXBUIFrame->ProcessKeyEvent(nKeyCode, bPress);
    }
    return false;
}

void CDlgXMLBrowser::OnFocusChanged(bool bFocused)
{
    if (NULL == m_pXBUIFrame)
    {
        return;
    }
    if (bFocused)
    {
        // 连接相关的信号槽
        SLOT_CONNECT(m_pXBUIFrame, m_signalFocusChange, this, CDlgXMLBrowser::OnChangeSoftkey);
    }
    else
    {
        // 断开相关的信号槽
        SLOT_DISCONNECT(m_pXBUIFrame, m_signalFocusChange, this, CDlgXMLBrowser::OnChangeSoftkey);
    }
}

void CDlgXMLBrowser::RefreshUI()
{
    if (NULL == m_pXBUIFrame)
    {
        return;
    }
    m_pXBUIFrame->RefreshUI();
}

bool CDlgXMLBrowser::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strPix)
{
    iVolumeType = VT_RING;
    iMin = RING_MIN_VALUE;
    iMax = RING_MAX_VALUE;
    return true;
}

void CDlgXMLBrowser::OnVolumeVisibleChanged(xSignal * sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    CBaseDialog * pDlg = UIManager_GetTopWindow();
    if (NULL == pDlg)
    {
        return;
    }
    if (eWT_XMLBrower == pDlg->GetWindowType())
    {
        if (wParam)
        {
#if IF_SUPPORT_BLUETOOTH
#ifndef _WIN32
#warning
#endif
//          if (!Bluetooth_IsMusicPlaying())
            {
#endif
                voice_PlayDefaultRing();
#if IF_SUPPORT_BLUETOOTH
            }
#endif
        }
        else
        {
            voice_StopRing();
        }
    }
}

void CDlgXMLBrowser::InitDialogData(const XMLLogicToUIData & objData)
{
    m_bFullScreen = false;
    UIManager_LockReturnToIdle(true);
    if (XMT_IMAGESCREEN == objData.eElementType
            || XMT_IMAGEMENU == objData.eElementType)
    {
        XBImageScreenData * pImageScreenData = static_cast<XBImageScreenData *>(objData.pElementData);
        if (NULL == pImageScreenData)
        {
            return;
        }
        m_bFullScreen = (IMAGE_MODE_FULLSCREEN == pImageScreenData->stAttr.eMode);
    }
}

#endif
