#ifndef XMLBROWSER_UI_MANAGER_H
#define XMLBROWSER_UI_MANAGER_H

#include "extended/xmlbrowser/include/logicanduicommon.h"

// 前向声明
class CWndTemplate;
class CXMLBrowserUIFrame;

#if XML_BROWSER_SUPPORT_UI

class CXMLBrowserUIManager
{
public:
    CXMLBrowserUIManager();

    ~CXMLBrowserUIManager();

    // 更新界面
    void UpdateXMLBrowserWnd();

    // 退出XMLBrowserDlg界面
    void ExitXMLBrowserDlg(int nElement, bool bUpdateWnd);
    // 退出所有UI
    void ExitXMLBrowserAll();
};

CXMLBrowserUIManager & GetXBUIManagerInstance();

#define XB_UIManager GetXBUIManagerInstance()

#endif

#endif // XMLBROWSER_UI_MANAGER_H
