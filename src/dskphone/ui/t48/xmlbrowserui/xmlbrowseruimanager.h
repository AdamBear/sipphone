#ifndef XMLBROWSER_UI_MANAGER_H
#define XMLBROWSER_UI_MANAGER_H

#include <map>
#include "extended/xmlbrowser/include/logicanduicommon.h"

// 前向声明
class CWndTemplate;
class CXMLBrowserUIFrame;

class CXMLBrowserUIManager
{
public:
    CXMLBrowserUIManager();

    ~CXMLBrowserUIManager();

    // 更新界面
    void UpdateXMLBrowserWnd();

    // 退出XMLBrowserDlg界面
    void ExitXMLBrowserDlg();

    // 激活Idle界面的XMLBrowser
    void ActivateXBIdle(bool bActivate);

    // 处理注册的消息
    LRESULT OnMyModeMessage(msgObject & refMessage);
};

// 接收消息并传递给CXMLBrowserUIManager的函数
LRESULT XMLBrowserLogicMessage(msgObject & refMessage);

CXMLBrowserUIManager & GetXBUIManagerInstance();

#define XB_UIManager GetXBUIManagerInstance()

#endif // XMLBROWSER_UI_MANAGER_H
