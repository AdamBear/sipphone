#ifndef __XML_BROWSER_MANAGER_H__
#define __XML_BROWSER_MANAGER_H__

#ifdef IF_FEATURE_XMLBROWSER

#include <singletonclassdefine.h>
#include "logicanduicommon.h"

#define XML_USER_NAME "$$SIPUSERNAME$$"
#define XML_REMOTE_NUM "$$REMOTENUMBER$$"

// 所有XMLBrowser逻辑部分的管理
class CXMLBrowserManager
{
    DECLARE_SINGLETON_CLASS(CXMLBrowserManager)
public:

    // 模块初始化
    void Init();

    // 定时器响应
    bool OnTimer(UINT uTimerID) DECLARE_ATTR;

    // 入口，默认要退出当前存在的Element(只有通过softkey进入上下级菜单不退出)
    void Enter(const char * lpszURL, bool bExitCurrent = true, bool bShowTips = true);
    // 整个XMLBrowser退出，逻辑对象都会被释放
    void Exit();

    // 对UI事件的处理
    bool OnAction(int nElementID, XB_ACTION_TYPE eAction, void * pData = NULL);

    // 更新界面
    void UpdateWnd();

    // 获取指定ID的对象数据
    bool GetData(XMLLogicToUIData & refToUIData);

    // 提示信息
    void ShowMessageBox(XB_TIP_TYPE eType, UINT uTimeout);

#if IF_FEATURE_XMLBROWSER_HOTELING
    // Hoteling请求
    bool HotelingRequest(bool bLogin);
    // 提示信息
    void ShowTips(const yl::string & strMessage, UINT uTimeout);
    // 是否是Hoteling登陆
    bool IsHotelingLogin()
    {
        return m_bHotelingLogin;
    }
#endif
    // 取消提示框
    void ExitMessageBox();

    // 是否允许显示XML
    bool IsAllowShowXML();

    // 取消下载
    void CancelDownLoad();

    // 删除逻辑对象的事件响应，可用于通知UI
    void OnDeleteElement(int nElement);

    // 播放提示音
    void PlayBeep();
    // 停止提示音
    void StopBeep();

    // 清除所有Element
    void DeleteAllElement();
    // 播放声音
    void PlayWav(const yl::string & strWavPath);
    void StopWav();

    // 替换变量符号为具体的值. 变量符号形如"$mac".
    // | strInput | 是输入字符串.
    // 替换字符串
    void MyReplaceVar(yl::string & strInput);


#if IF_BUG_30385
    void RequestAutopXml();
#endif

    void SetEnterDsskeyId(int nDssekyId)
    {
        m_nEnterDsskeyId = nDssekyId;
    }

    int GetEnterDsskeyId()
    {
        return m_nEnterDsskeyId;
    }

    // 处理主线程消息
    bool ParseFile(const yl::string & strFilename);
    void EnableTips(bool bEnabled = true)
    {
        m_bShowTips = bEnabled;
    }

    void CloseLoading();

    // 设置URL中的通话信息
    bool SetTalkInfoURL(yl::string & strURL);

#if IF_BUG_32431
    bool SetAutoRefresh(const int & nTimeOut, const yl::string & strURL);
    bool RefreshURL(UINT uTimerID);
    int GetURLIndex(const yl::string & strURL);
    bool StopAutoRefresh(const yl::string & strURL);
#endif

#if IF_FEATURE_XMLB_SCREENSAVER
    // 获取xml屏保数据
    bool GetSSData(XMLLogicToUIData & objToUIData);
    // 清除xml屏保缓存数据
    void DeleteSSData();
#endif

private:
    // 处理主线程消息
    static LRESULT ProcessMSG(msgObject & msg);
    // 定时器响应
    static LRESULT OnTimer(msgObject & objMsg) DECLARE_ATTR;

private:
    // 用于停止播放xml中的声音
    UINT    m_uPlayWavTimer;
    // 播放的声音文件
    yl::string  m_strWavFile;

    int m_nEnterDsskeyId;

    bool    m_bShowTips;

#if IF_BUG_32431
    YLVector<yl::string> vecURL;
#endif

#if IF_FEATURE_XMLBROWSER_HOTELING
    bool m_bHotelingLogin;
#endif
};

// 获取全局唯一的XMLBrowser逻辑部分管理对象
#define _XMLBrowserManager GET_SINGLETON_INSTANCE(CXMLBrowserManager)

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif // __XML_BROWSER_MANAGER_H__
