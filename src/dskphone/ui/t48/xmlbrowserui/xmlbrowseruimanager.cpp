// mod this
#include "xmlbrowseruimanager.h"
#include "cdlgxmlbrowser.h"

// mod XmlBrowserLogic
#include "extended/xmlbrowser/include/modxmlbrowser.h"

// mod MainWnd
#include "messagebox/modmessagebox.h"

// mod IdleScreenUI
#include "idlescreenui/modidlescreenui.h"

//#include "uimanager/moduimanager.h"

// mod KeyGuard
#include "keyguard/include/modkeyguard.h"
#include "dsklog/log.h"

#if IF_FEATURE_XMLB_SCREENSAVER
#include "screensavers/include/modscreensavers.h"
#endif


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

    ///----T48 键盘锁 phone lock
    if (!bRefresh
            /*|| !keyGuard_IsEnableModuld(MODULE_XMLBROWSER)*/)
    {
        return ;
    }

#if IF_FEATURE_XMLB_SCREENSAVER
    if (objData.eElementType == XMT_SCREENSAVER)
    {
        ScreenSavers_XMBEnter(objData.pElementData);
        return;
    }
#endif

    //主动清除弹出框
    MessageBox_RemoveAllMessageBox(MessageBox_Destory);

    // 获取XMLBrowser的弹窗
    CDlgXMLBrowser * pDlgXB = qobject_cast<CDlgXMLBrowser *>(UIManager_GetPrivateDlg(
                                  DLG_CDlgXMLBrowser));
    if (NULL == pDlgXB)
    {
        pDlgXB = qobject_cast<CDlgXMLBrowser *>(UIManager_CreatePrivateDlg(DLG_CDlgXMLBrowser,
                                                DLG_CDlgXMLBrowser));
    }

    if (NULL == pDlgXB)
    {
        XMLBROWSERUI_WARN("Create XML Browser Dialog error!");
        return;
    }

    pDlgXB->SetData((void *)&objData); // 初始化数据
    UIManager_UpdateWnd(pDlgXB);     // 刷新到界面上
    pDlgXB->UpdateUI(objData);       // 刷新数据
}

void CXMLBrowserUIManager::ExitXMLBrowserDlg()
{
    CBaseDialog * pBaseDlg = UIManager_GetPrivateDlg(DLG_CDlgXMLBrowser);
    if (NULL != pBaseDlg)
    {
        UIManager_PutPrivateDlg(pBaseDlg);
    }
}

void CXMLBrowserUIManager::ActivateXBIdle(bool bActivate)
{

}


// 处理注册的消息
LRESULT CXMLBrowserUIManager::OnMyModeMessage(msgObject & refMessage)
{
    return 0;
}

// 接收消息并传递给CXMLBrowserUIManager的函数
LRESULT XMLBrowserLogicMessage(msgObject & refMessage)
{
    return XB_UIManager.OnMyModeMessage(refMessage);
}
