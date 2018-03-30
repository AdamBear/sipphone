#include "modadapterbox.h"
#include "adapterbox.h"
#include <ETLLib.hpp>

void AdapterBox_UploadConfig()
{
    if (_AdapterBox.m_FunUploadConfig != NULL)
    {
        _AdapterBox.m_FunUploadConfig();
    }
}

void AdapterBox_UpdateFirware()
{
    if (_AdapterBox.m_FunUpdateFirmware != NULL)
    {
        _AdapterBox.m_FunUpdateFirmware();
    }
}

// 返回Idle
void AdapterBox_ReturnToIdle(bool bClickToIdle/* = false*/)
{
    if (_AdapterBox.m_FunReturnToIdle != NULL)
    {
        _AdapterBox.m_FunReturnToIdle(bClickToIdle);
    }
}

void AdapterBox_UpdateTopWnd()
{
    if (_AdapterBox.m_FunUpdateTopWnd != NULL)
    {
        _AdapterBox.m_FunUpdateTopWnd();
    }
}

void AdapterBox_SendKey(int nKeyCode, bool bPress)
{
    if (NULL != _AdapterBox.m_FunSendKey)
    {
        _AdapterBox.m_FunSendKey(nKeyCode, bPress);
    }
}

// 清空话路数据
void AdapterBox_TalkUIClearSessionData(int iSessionID)
{
    if (_AdapterBox.m_FunTalkUIClearSessionData != NULL)
    {
        _AdapterBox.m_FunTalkUIClearSessionData(iSessionID);
    }
}

// 话路退出
void AdapterBox_TalkUISessionExit(int iSessionID)
{
    if (_AdapterBox.m_FunTalkUISessionExit != NULL)
    {
        _AdapterBox.m_FunTalkUISessionExit(iSessionID);
    }
}

// 获取通话时长
int AdapterBox_TalkUIGetTalkTimeBySessionId(int iSessionID)
{
    if (_AdapterBox.m_FunTalkUIGetTalkTimeBySessionId != NULL)
    {
        return _AdapterBox.m_FunTalkUIGetTalkTimeBySessionId(iSessionID);
    }
    return 0;
}

// 获取输入内容
yl::string AdapterBox_TalkUIGetInputContent()
{
    if (_AdapterBox.m_FunTalkUIGetInputContent != NULL)
    {
        return _AdapterBox.m_FunTalkUIGetInputContent();
    }
    return "";
}

// 获取不包括预输入状态的内容
yl::string AdapterBox_TalkUIGetAlreadyInputContent()
{
    if (_AdapterBox.m_FunTalkUIGetAlreadyInputContent != NULL)
    {
        return _AdapterBox.m_FunTalkUIGetAlreadyInputContent();
    }
    return "";
}

// 获取输入内容
bool AdapterBox_TalkUIGetInputContent(yl::string & strNum, yl::string & strDisplayname)
{
    if (_AdapterBox.m_FunTalkUIGetInputContentEx != NULL)
    {
        return _AdapterBox.m_FunTalkUIGetInputContentEx(strNum, strDisplayname);
    }
    return false;
}

// 获取输入内容
int AdapterBox_TalkUIGetInputCallID()
{
    if (_AdapterBox.m_FunTalkUIGetInputCallID != NULL)
    {
        return _AdapterBox.m_FunTalkUIGetInputCallID();
    }
    return -1;
}

// 更新talkui
void AdapterBox_TalkUIUpdateWnd(void * pData/* = NULL*/)
{
    if (_AdapterBox.m_FunTalkUIUpdateWnd != NULL)
    {
        _AdapterBox.m_FunTalkUIUpdateWnd(pData);
    }
}

// 更新talkui错误信息
void AdapterBox_TalkUIUpdateErrorInfo(void * pData/* = NULL*/)
{
    if (_AdapterBox.m_FunTalkUIUpdateErrorInfo != NULL)
    {
        _AdapterBox.m_FunTalkUIUpdateErrorInfo(pData);
    }
}

// 是否是在通话界面
bool AdapterBox_TalkUIIsInTalk()
{
    if (_AdapterBox.m_FunTalkUIIsInTalk)
    {
        return _AdapterBox.m_FunTalkUIIsInTalk();
    }

    return false;
}

// 进入SettingUI界面(ACD用)
void AdapterBox_SettingUIEnterPage(int iType, int iFocusIndex/* = 0*/)
{
    if (_AdapterBox.m_FunSettingUIEnterPage != NULL)
    {
        _AdapterBox.m_FunSettingUIEnterPage(iType, iFocusIndex);
    }
}

void AdapterBox_DirUIEnterLocalMenuPage()
{
    if (_AdapterBox.m_FunDirUIEnterLocalMenuPage != NULL)
    {
        _AdapterBox.m_FunDirUIEnterLocalMenuPage();
    }
}

// 判断是否测试模式
bool AdapterBox_IsTestMode()
{
    if (_AdapterBox.m_FunIsTestMode != NULL)
    {
        return _AdapterBox.m_FunIsTestMode();
    }
    return false;
}

// 进入测试模式
void AdapterBox_EnterTestMode(int iTestType)
{
    if (_AdapterBox.m_FunEnterTestMode != NULL)
    {
        _AdapterBox.m_FunEnterTestMode(iTestType);
    }
}

// 退出测试模式
void AdapterBox_ExitTestMode()
{
    if (_AdapterBox.m_FunExitTestMode != NULL)
    {
        _AdapterBox.m_FunExitTestMode();
    }
}

// xmlBroser弹窗提示
void AdapterBox_XmlBrowserShowMessage(int iTipType, UINT uTimeout)
{
    if (_AdapterBox.m_FunXmlBrowserShowMessage != NULL)
    {
        _AdapterBox.m_FunXmlBrowserShowMessage(iTipType, uTimeout);
    }
}

#if IF_FEATURE_XMLBROWSER_HOTELING
// xmlBroser弹窗提示
void AdapterBox_XmlBrowserShowTips(const yl::string & strMessage, UINT uTimeout)
{
    if (_AdapterBox.m_FunXmlBrowserShowTips != NULL)
    {
        _AdapterBox.m_FunXmlBrowserShowTips(strMessage, uTimeout);
    }
}
#endif

// xmlBroser摧毁弹窗
void AdapterBox_XmlBrowserDestroyMessage()
{
    if (_AdapterBox.m_FunXmlBrowserDestroyMessage != NULL)
    {
        _AdapterBox.m_FunXmlBrowserDestroyMessage();
    }
}

// xmlBroser刷新UI界面
void AdapterBox_XmlBrowserUpdateWnd()
{
    if (_AdapterBox.m_FunXmlBrowserUpdateWnd != NULL)
    {
        _AdapterBox.m_FunXmlBrowserUpdateWnd();
    }
}

// xmlBroser退出某界面
void AdapterBox_XmlBrowserExitXBDlg(int iIndex, bool bUpdate)
{
    if (_AdapterBox.m_FunXmlBrowserExitXBDlg != NULL)
    {
        _AdapterBox.m_FunXmlBrowserExitXBDlg(iIndex, bUpdate);
    }
}

// xmlBroser点Dsskey灯
void AdapterBox_XmlBrowserCtrlLight(int iDsskeyIndex, int iColorType, int iFlushType)
{
    if (_AdapterBox.m_FunXmlBrowserCtrlLight != NULL)
    {
        _AdapterBox.m_FunXmlBrowserCtrlLight(iDsskeyIndex, iColorType, iFlushType);
    }
}

// 显示sca菜单
bool AdapterBox_ShowSCAMenu(void * pData)
{
    if (_AdapterBox.m_FunShowSCAMenu != NULL)
    {
        return _AdapterBox.m_FunShowSCAMenu(pData);
    }
    return false;
}

// 当前是否已显示SCA菜单
bool AdapterBox_IsExistSCAMenu()
{
    if (_AdapterBox.m_FunIsExistSCAMenu != NULL)
    {
        return _AdapterBox.m_FunIsExistSCAMenu();
    }
    return false;
}

// 退出SCA菜单
void AdapterBox_ExitSCAMenu()
{
    if (_AdapterBox.m_FunExitSCAMenu != NULL)
    {
        _AdapterBox.m_FunExitSCAMenu();
    }
}

#if IF_FEATURE_SHOW_BLF_CALLID
bool AdapterBox_ShowBLFMenu(void* pData)
{
    if (_AdapterBox.m_FunShowBLFMenu != NULL)
    {
        return _AdapterBox.m_FunShowBLFMenu(pData);
    }
    return false;
}

bool AdapterBox_IsExistBLFMenu(const yl::string& strKey)
{
    if (_AdapterBox.m_FunIsExistBLFMenu != NULL)
    {
        return _AdapterBox.m_FunIsExistBLFMenu(strKey);
    }
    return false;
}

void AdapterBox_ExitBLFMenu()
{
    if (_AdapterBox.m_FunExitBLFMenu != NULL)
    {
        _AdapterBox.m_FunExitBLFMenu();
    }
}
#endif

// 退出pickupmenu界面
void AdapterBox_ExitPickupMenu()
{
    if (_AdapterBox.m_FunExitPickupMenu != NULL)
    {
        _AdapterBox.m_FunExitPickupMenu();
    }

}

bool AdapterBox_EnterConnectVoiceMailPage(int iAccountID/* = -1*/)
{
    if (_AdapterBox.m_FunEnterConnectVoiceMailPage != NULL)
    {
        return _AdapterBox.m_FunEnterConnectVoiceMailPage(iAccountID);
    }
    return false;
}

void AdapterBox_ReloadIME()
{
    if (_AdapterBox.m_FunReloadIME != NULL)
    {
        _AdapterBox.m_FunReloadIME();
    }
}

void AdapterBox_Reboot(int iTimeDelay)
{
    if (_AdapterBox.m_FunReboot != NULL)
    {
        _AdapterBox.m_FunReboot(iTimeDelay);
    }
}

void AdapterBox_ResetFactory(int iResetType)
{
    if (_AdapterBox.m_FunResetFactory != NULL)
    {
        _AdapterBox.m_FunResetFactory(iResetType);
    }
}

void AdapterBox_XmlBrowserUpdate()
{
    if (_AdapterBox.m_FunXmlBrowserUpdate != NULL)
    {
        _AdapterBox.m_FunXmlBrowserUpdate();
    }
}

bool AdapterBox_IsInIdle()
{
    if (_AdapterBox.m_FunIsInIdle != NULL)
    {
        return _AdapterBox.m_FunIsInIdle();
    }
    return false;
}

bool AdapterBox_VoiceMailCallOut(int iIndex)
{
    if (_AdapterBox.m_FunVoiceMailCallOut != NULL)
    {
        return _AdapterBox.m_FunVoiceMailCallOut(iIndex);
    }

    return false;
}

int AdapterBox_VoiceMailGetUnReadNum(int iAccountID/* = -1*/, bool bNotify/* = false*/)
{
    if (_AdapterBox.m_FunVoiceMailGetUnReadNum != NULL)
    {
        return _AdapterBox.m_FunVoiceMailGetUnReadNum(iAccountID, bNotify);
    }

    return 0;
}

// 获取视频Mute图片
yl::string AdapterBox_GetMutePicture(int iResolutionType)
{
    if (_AdapterBox.m_FunGetMutePicture != NULL)
    {
        return _AdapterBox.m_FunGetMutePicture(iResolutionType);
    }
    return "";
}

bool AdatapterBox_EnterCameraPage()
{
    if (_AdapterBox.m_FunEnterCameraPage != NULL)
    {
        return _AdapterBox.m_FunEnterCameraPage();
    }

    return false;
}
bool AdatapterBox_IsInTalkUIFrame()
{
    if (_AdapterBox.m_FunIsInTalkUIFrame != NULL)
    {
        return _AdapterBox.m_FunIsInTalkUIFrame();
    }
    return false;
}

void AdapterBox_ShowEdkPrompt(int iIndex, int iId, bool(*pFun)(bool, int, yl::string &))
{
    if (_AdapterBox.m_FunShowEdkPrompt != NULL)
    {
        _AdapterBox.m_FunShowEdkPrompt(iIndex, iId, pFun);
    }
}

void AdapterBox_UpdateSoftKey()
{
    if (_AdapterBox.m_FunUpdateSoftKey != NULL)
    {
        _AdapterBox.m_FunUpdateSoftKey();
    }
}

bool AdapterBox_IsInTalk()
{
    if (_AdapterBox.m_FunIsInTalk != NULL)
    {
        return _AdapterBox.m_FunIsInTalk();
    }
    return false;
}

void AdapterBox_InsertSoftKey(YLList<SoftKey_Data> & listSoftKey)
{
    if (_AdapterBox.m_FunInsertSoftKey != NULL)
    {
        _AdapterBox.m_FunInsertSoftKey(listSoftKey);
    }
}


BOOL AdapterBox_IsTopTipUI()
{
    if (_AdapterBox.m_FunIsTopTipUI != NULL)
    {
        return _AdapterBox.m_FunIsTopTipUI();
    }
    return FALSE;
}

BOOL AdapterBox_IsTopMessageBoxUI()
{
    if (_AdapterBox.m_FunIsTopMessageBoxUI != NULL)
    {
        return _AdapterBox.m_FunIsTopMessageBoxUI();
    }
    return FALSE;
}

// 是否处于设置向导中
BOOL AdapterBox_IsIsInWizard()
{
    if (_AdapterBox.m_FunIsInWizard != NULL)
    {
        return _AdapterBox.m_FunIsInWizard();
    }

    return FALSE;
}


// enter zero interface
bool AdapterBox_ZeroOpenDialog(const yl::string & strPageAction, bool bRepaint /*= false*/)
{
    if (_AdapterBox.m_FunZeroOpenDialog != NULL)
    {
        return _AdapterBox.m_FunZeroOpenDialog(strPageAction, bRepaint);
    }
    return false;
}

// enter rps dialog
bool AdapterBox_ZeroEnterRPS()
{
    if (_AdapterBox.m_FunZeroEnterPRS != NULL)
    {
        return _AdapterBox.m_FunZeroEnterPRS();
    }
    return false;
}

// hide notifycenter
void AdapterBox_HideNotifyCenter()
{
    if (_AdapterBox.m_FunHideNotifyCenter != NULL)
    {
        _AdapterBox.m_FunHideNotifyCenter();
    }
    return;
}

// 显示autop成功或失败界面
void AdapterBox_ShowAutopResult(bool bSuccess)
{
    if (_AdapterBox.m_FunShowAutopResult != NULL)
    {
        _AdapterBox.m_FunShowAutopResult(bSuccess);
    }
}

void AdapterBox_EnterRPS()
{
    if (_AdapterBox.m_FunEnterRPS != NULL)
    {
        _AdapterBox.m_FunEnterRPS();
    }
}

void AdapterBox_ZeroOpenDlg(const int iType)
{
    if (_AdapterBox.m_FunZEROOpenDlg != NULL)
    {
        _AdapterBox.m_FunZEROOpenDlg(iType);
    }
}

void AdapterBox_CreateMessageBox(void * pLister, const char * cszTitle, const char * cszContent,
                                 int eBtnType, unsigned int uExitTime, int iTrigerType, int eRegionType)
{
    if (_AdapterBox.m_FunCreateMessageBox != NULL)
    {
        _AdapterBox.m_FunCreateMessageBox(pLister, cszTitle, cszContent, eBtnType, uExitTime, iTrigerType,
                                          eRegionType);
    }
}

void AdapterBox_CreateMenuTips(void * pListener, int eTipsType, const char * cszContent1,
                               const char * cszContent2, unsigned int uExitTime, int iTrigerType /* = -1 */)
{
    if (_AdapterBox.m_FunCreateMenuTips != NULL)
    {
        _AdapterBox.m_FunCreateMenuTips(pListener, eTipsType, cszContent1, cszContent2, uExitTime,
                                        iTrigerType);
    }
}

void AdapterBox_ExitTriggers(void * pListener, int iTriggerType, BOOL bUpdate /* = TRUE */)
{
    if (_AdapterBox.m_FunExitTriggers != NULL)
    {
        _AdapterBox.m_FunExitTriggers(pListener, iTriggerType, bUpdate);
    }
}

bool AdapterBox_EnterMenu(const yl::string & strMenuID, int iIndex/* = 0*/)
{
    if (_AdapterBox.m_FunEnterMenu != NULL)
    {
        return _AdapterBox.m_FunEnterMenu(strMenuID, iIndex);
    }
    return false;
}

bool AdapterBox_ExecuteSoftkey(const yl::string & strSoftkeyID)
{
    if (_AdapterBox.m_FunExecuteSoftkey != NULL)
    {
        return _AdapterBox.m_FunExecuteSoftkey(strSoftkeyID);
    }
    return false;
}

void AdapterBox_ShowVolumeBar(bool bEnable)
{
    if (_AdapterBox.m_FunShowVolumeBar != NULL)
    {
        _AdapterBox.m_FunShowVolumeBar(bEnable);
    }
}

void AdapterBox_ExitScreenSaver()
{
    if (_AdapterBox.m_FunExitScreenSaver != NULL)
    {
        _AdapterBox.m_FunExitScreenSaver();
    }
    return;
}


// enter zero AuthenticationAgain
bool AdapterBox_ZeroAuthenticationAgain()
{
    if (_AdapterBox.m_FunZeroAuthenticationAgain != NULL)
    {
        return _AdapterBox.m_FunZeroAuthenticationAgain();
    }
    return false;
}

void AdapterBox_ShowDsskeyLockPassword(int iDsskeyID)
{
    if (_AdapterBox.m_FunShowDsskeyLockPassword != NULL)
    {
        _AdapterBox.m_FunShowDsskeyLockPassword(iDsskeyID);
    }
    return;
}


void AdapterBox_ExitDsskeyDlg()
{
    if (_AdapterBox.m_FunExitDsskeyDlg != NULL)
    {
        _AdapterBox.m_FunExitDsskeyDlg();
    }
}

int AdapterBox_ImageScale(const yl::string & strSrcFile, const yl::string & strDstFile,
                          int eImagetype, int nScaleXDef, int nScaleYDef)
{
    int iResult = -1;
    if (_AdapterBox.m_FunImageScale != NULL)
    {
        iResult = _AdapterBox.m_FunImageScale(strSrcFile, strDstFile, eImagetype, nScaleXDef, nScaleYDef);
    }
    return iResult;
}

// enter conf with
void AdapterBox_EnterConfWith(int iSessionID)
{
    if (_AdapterBox.m_FunTalkUIEnterConfWith != NULL)
    {
        _AdapterBox.m_FunTalkUIEnterConfWith(iSessionID);
    }
}

int AdapterBox_GetGetTotalUnReadMessage()
{
    if (_AdapterBox.m_FunGetTotalUnReadMessage != NULL)
    {
        return _AdapterBox.m_FunGetTotalUnReadMessage();
    }

    return false;
}

bool AdapterBox_DsskeyLongPress(int iDsskeyID)
{
    if (_AdapterBox.m_FunDsskeyLongPress != NULL)
    {
        return _AdapterBox.m_FunDsskeyLongPress(iDsskeyID);
    }
    return false;
}
