// mod this
#include "xmlbrowseruimanager.h"
#include "dlgxmlbrowser.h"

// mod XmlBrowserLogic
#include "extended/xmlbrowser/include/modxmlbrowser.h"

// mod MainWnd
#include "messagebox/modmessagebox.h"

// mod IdleScreenUI
#include "idlescreenui/modidleui.h"
#include "uimanager/moduimanager.h"

// mod KeyGuard
#include "keyguard/include/modkeyguard.h"
#include "dsklog/log.h"
#include "xwindow/xWindowManager.h"
#include "screensavers/include/modscreensavers.h"

#if XML_BROWSER_SUPPORT_UI

CXMLBrowserUIManager & GetXBUIManagerInstance()
{
    static CXMLBrowserUIManager s_XBUIManager;
    return s_XBUIManager;
}

CXMLBrowserUIManager::CXMLBrowserUIManager()
{

}

CXMLBrowserUIManager::~CXMLBrowserUIManager()
{

}

void CXMLBrowserUIManager::UpdateXMLBrowserWnd()
{
    // 获取逻辑层数据
    XMLLogicToUIData objData;
    bool bRefresh = xmlbrowser_GetData(objData);

    // 无活动对象，退出窗口
    if (!bRefresh || INVALID_ELEMENT_ID == objData.nElementID)
    {
        ExitXMLBrowserAll();
        return;
    }

    if (objData.eElementType == XMT_SCREENSAVER)
    {
#if IF_FEATURE_XMLB_SCREENSAVER
        ScreenSavers_XMBLoad();
        return;
#endif
    }

    //主动清除弹出框
    MessageBox_RemoveTopMessageBox(MessageBox_Destory);

    // 获取XMLBrowser的弹窗
    CDlgXMLBrowser * pDlgXB = static_cast<CDlgXMLBrowser *>(UIManager_GetPrivateDlg(
                                  DLG_CDlgXMLBrowser));
    if (NULL == pDlgXB)
    {
        pDlgXB = static_cast<CDlgXMLBrowser *>(UIManager_CreatePrivateDlg(DLG_CDlgXMLBrowser,
                                               DLG_CDlgXMLBrowser));
    }

    if (NULL == pDlgXB)
    {
        XMLBROWSERUI_WARN("Create XML Browser Dialog error!");
        return;
    }

    pDlgXB->InitDialogData(objData); // 初始化数据
    UIManager_UpdateWnd(pDlgXB);     // 刷新到界面上
    pDlgXB->UpdateUI(objData);       // 刷新数据
    pDlgXB->RefreshUI();
}

void CXMLBrowserUIManager::ExitXMLBrowserDlg(int nElement, bool bUpdateWnd)
{
    // exit ui frame
    CBaseDialog* pBaseDlg = UIManager_GetPrivateDlg(DLG_CDlgXMLBrowser);
    if (NULL == pBaseDlg)
    {
        return;
    }
    if (INVALID_ELEMENT_ID == nElement)
    {
        // exit ui frame
        UIManager_PutPrivateDlg(pBaseDlg);
    }
    else
    {
        // exit ui
        CDlgXMLBrowser* pDlgXB = static_cast<CDlgXMLBrowser*>(pBaseDlg);
        pDlgXB->ExitXmlUI(nElement);
    }

    if (bUpdateWnd)
    {
        UpdateXMLBrowserWnd();
    }
}
void CXMLBrowserUIManager::ExitXMLBrowserAll()
{
    CBaseDialog* pBaseDlg = UIManager_GetPrivateDlg(DLG_CDlgXMLBrowser);
    if (NULL == pBaseDlg)
    {
        return;
    }
    UIManager_PutPrivateDlg(pBaseDlg);
}

#endif
