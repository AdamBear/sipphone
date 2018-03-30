#include "logic_inc.h"
#include "dsskey/include/idsskeyimplaction.h"
#include "xmldsskeyaction.h"

#include "modxmlbrowser.h"
#include "xmlbrowsermanager.h"
#include "elementmanager.h"
#include "adapterbox/include/modadapterbox.h"

CDssKeyXMLBrowserAction * s_pXMLBrowserAction = NULL;

#ifdef IF_FEATURE_XMLBROWSER
// 初始化
void xmlbrowser_Init()
{
    _XMLBrowserManager.Init();
    s_pXMLBrowserAction = new CDssKeyXMLBrowserAction();
}

// 入口，默认要退出当前存在的Element(只有通过softkey进入上下级菜单不退出)
void xmlbrowser_Enter(const char * lpszURL, bool bExitCurrent, int nDsskeyId/* = -1*/,
                      bool bShowTips /*= true*/)
{
    if (!_XMLBrowserManager.IsAllowShowXML())
    {
        return;
    }

    if (bExitCurrent
            && !talklogic_SessionExist())
    {
        // 退出非通话界面

        //如果有xml界面先不退出
        if (!_ElementManager.IsHaveXBElement())
        {
            AdapterBox_ReturnToIdle();
        }

    }

    _XMLBrowserManager.SetEnterDsskeyId(nDsskeyId);

    _XMLBrowserManager.Enter(lpszURL, bExitCurrent, bShowTips);
}

// 退出XMLBrowser接口
void xmlbrowser_Exit()
{
    _XMLBrowserManager.Exit();
}

// 获取当前数据及界面类型
bool xmlbrowser_GetData(XMLLogicToUIData & refToUIData)
{
    return _XMLBrowserManager.GetData(refToUIData);
}

// 对UI事件的处理
bool xmlbrowser_OnAction(int nElementID, XB_ACTION_TYPE eAction, void * pData /*= NULL*/)
{
    return _XMLBrowserManager.OnAction(nElementID, eAction, pData);
}

// 清除所有数据
void xmlbrowser_CleanAllData()
{
    // 删除所有element
    _XMLBrowserManager.DeleteAllElement();
}

// 隐藏模块
void xmlbrowser_HideXmlBrowser(bool bHide)
{
    if (!bHide)
    {
        // 有ui界面才去刷新，避免每次通话结束就去刷新一个界面
        if (xmlbrowser_IsHaveXBElement())
        {
            // 如果取消隐藏就要刷新界面
            XMLBROWSER_INFO("xmlbrowser_HideXmlBrowser UpdateWnd");
            _XMLBrowserManager.UpdateWnd();
        }
    }
}

// 取消下载
void xmlbrowser_CancelDownload()
{
    _XMLBrowserManager.CancelDownLoad();
}

bool xmlbrowser_IsHaveXBElement()
{
    return _ElementManager.IsHaveXBElement();
}

#if IF_BUG_30385
void xmlbrowser_RequestAutopXml()
{
    _XMLBrowserManager.RequestAutopXml();
}
#endif

// 解析XML文件
bool xmlbrowser_ParseFile(const yl::string & strFilename)
{
    if (_XMLBrowserManager.IsAllowShowXML())
    {
        return _XMLBrowserManager.ParseFile(strFilename);
    }

    return true;
}

void xmlbrowser_EnableTips(bool bEnabled /*= false*/)
{
    _XMLBrowserManager.EnableTips(bEnabled);
}

void xmlbrowser_CloseLoading()
{
    _XMLBrowserManager.CloseLoading();
}

void xmlbrowser_ReleaseStatus(const yl::string & strSessionId)
{
    CElementStatus::ReleaseStatus(strSessionId);
}

#if IF_FEATURE_XMLBROWSER_HOTELING
bool xmlbrowser_HotelingRequest(bool bLogin)
{
    return _XMLBrowserManager.HotelingRequest(bLogin);
}

bool xmlbrowser_IsHotelingLogin()
{
    return _XMLBrowserManager.IsHotelingLogin();
}
#endif
#if IF_FEATURE_XMLB_SCREENSAVER
void xmlbrowser_ScreenSaverEnter(const char * lpszURL)
{
    if (!_XMLBrowserManager.IsAllowShowXML())
    {
        return;
    }

    //不显示loading提示
    _XMLBrowserManager.Enter(lpszURL, true, false);
}
bool xmlbrowser_GetSSData(XMLLogicToUIData & objToUIData)
{
    if (!_XMLBrowserManager.IsAllowShowXML())
    {
        return false;
    }

    return _XMLBrowserManager.GetSSData(objToUIData);
}
//删除xmlbrowser模式屏保缓存数据
void xmlbrowser_DeleteSSData()
{
    _XMLBrowserManager.DeleteSSData();
}
#endif //#if IF_FEATURE_XMLB_SCREENSAVER

#else // #ifdef IF_FEATURE_XMLBROWSER

// 初始化
void xmlbrowser_Init()
{
}

// 入口，默认要退出当前存在的Element(只有通过softkey进入上下级菜单不退出)
void xmlbrowser_Enter(const char * lpszURL, bool bExitCurrent, int nDsskeyId)
{
}

// 退出XMLBrowser接口
void xmlbrowser_Exit()
{
}

// 获取当前数据及界面类型
bool xmlbrowser_GetData(XMLLogicToUIData & refToUIData)
{
    return false;
}

// 对UI事件的处理
bool xmlbrowser_OnAction(int nElementID, XB_ACTION_TYPE eAction, void * pData)
{
    return false;
}

// 清除所有数据
void xmlbrowser_CleanAllData()
{
}

// 隐藏模块
void xmlbrowser_HideXmlBrowser(bool bHide)
{
}

// 取消下载
void xmlbrowser_CancelDownload()
{
}

bool xmlbrowser_IsHaveXBElement()
{
    return true;
}

#if IF_BUG_30385
void xmlbrowser_RequestAutopXml()
{
}
#endif

#if IF_FEATURE_XMLBROWSER_HOTELING
bool xmlbrowser_HotelingRequest(bool bLogin)
{
    return false;
}

bool xmlbrowser_IsHotelingLogin()
{
    return false;
}
#endif
#if IF_FEATURE_XMLB_SCREENSAVER
void xmlbrowser_ScreenSaverEnter(const char * lpszURL)
{

}
bool xmlbrowser_GetSSData(XMLLogicToUIData & objToUIData)
{
    return true;
}
void xmlbrowser_DeleteSSData()
{

}
#endif //#if IF_FEATURE_XMLB_SCREENSAVER
#endif
