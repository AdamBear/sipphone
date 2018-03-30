#ifndef __MOD_ADAPTERBOX_H__
#define __MOD_ADAPTERBOX_H__

#include "adapterboxdefine.h"

void AdapterBox_UploadConfig();

void AdapterBox_UpdateFirware();

// 返回Idle
void AdapterBox_ReturnToIdle(bool bClickToIdle = false);

// 更新界面
void AdapterBox_UpdateTopWnd();

// 模拟发送硬按键
void AdapterBox_SendKey(int nKeyCode, bool bPress);

// 清空话路数据
void AdapterBox_TalkUIClearSessionData(int iSessionID);
// 话路退出
void AdapterBox_TalkUISessionExit(int iSessionID);
// 获取通话时长
int AdapterBox_TalkUIGetTalkTimeBySessionId(int iSessionID);
// 获取输入内容
yl::string AdapterBox_TalkUIGetInputContent();
// 获取输入内容
bool AdapterBox_TalkUIGetInputContent(yl::string & strNum, yl::string & strDisplayname);
// 获取输入号码的CallID
int AdapterBox_TalkUIGetInputCallID();

// 获取不包括预输入状态的内容
yl::string AdapterBox_TalkUIGetAlreadyInputContent();
// 更新talkui
void AdapterBox_TalkUIUpdateWnd(void * pData = NULL);
// 更新talkui错误信息
void AdapterBox_TalkUIUpdateErrorInfo(void * pData = NULL);
bool AdapterBox_TalkUIIsInTalk();

// 进入SettingUI界面(ACD用)
void AdapterBox_SettingUIEnterPage(int iType, int iFocusIndex = 0);

//进入联系人UI界面
void AdapterBox_DirUIEnterLocalMenuPage();

// 判断是否测试模式
bool AdapterBox_IsTestMode();

// 判断是否测试模式
void AdapterBox_EnterTestMode(int iTestType);

// 判断是否测试模式
void AdapterBox_ExitTestMode();

// xmlBroser弹窗提示
void AdapterBox_XmlBrowserShowMessage(int iTipType, UINT uTimeout);

#if IF_FEATURE_XMLBROWSER_HOTELING
// xmlBroser弹窗提示
void AdapterBox_XmlBrowserShowTips(const yl::string & strMessage, UINT uTimeout);
#endif

// xmlBroser弹窗提示
void AdapterBox_XmlBrowserDestroyMessage();

// xmlBroser刷新UI界面
void AdapterBox_XmlBrowserUpdateWnd();

// xmlBroser退出某界面
void AdapterBox_XmlBrowserExitXBDlg(int iIndex, bool bUpdate);

// xmlBroser点Dsskey灯
void AdapterBox_XmlBrowserCtrlLight(int iDsskeyIndex, int iColorType, int iFlushType);

// 显示sca菜单
bool AdapterBox_ShowSCAMenu(void * pData);

// 当前是否已显示SCA菜单
bool AdapterBox_IsExistSCAMenu();

// 退出SCA菜单
void AdapterBox_ExitSCAMenu();

#if IF_FEATURE_SHOW_BLF_CALLID
// 显示blf菜单
bool AdapterBox_ShowBLFMenu(void* pData);

// 当前是否已显示blf菜单
bool AdapterBox_IsExistBLFMenu(const yl::string& strKey);

// 退出blf菜单
void AdapterBox_ExitBLFMenu();
#endif

void AdapterBox_ExitPickupMenu();
// 进入ConnectVoiceMail界面
bool AdapterBox_EnterConnectVoiceMailPage(int iAccountID = -1);

// 重新加载输入法
void AdapterBox_ReloadIME();

// 重启前UI执行的操作
void AdapterBox_Reboot(int iTimeDelay);

// 恢复出厂前UI执行的操作
void AdapterBox_ResetFactory(int iResetType);

// 更新XMLBrowser数据
void AdapterBox_XmlBrowserUpdate();

// 当前是否处于Idle界面
bool AdapterBox_IsInIdle();

//Voicemail 呼出
bool AdapterBox_VoiceMailCallOut(int iIndex);

//获取Voicemail 未读数量
int AdapterBox_VoiceMailGetUnReadNum(int iAccountID = -1, bool bNotify = false);

// 或者视频Mute图片
yl::string AdapterBox_GetMutePicture(int iResolutionType);

//进入Camara 界面
bool AdatapterBox_EnterCameraPage();

bool AdatapterBox_IsInTalkUIFrame();

// 当前是否处于Idle界面
bool AdapterBox_IsInTalk();

// 弹出edk输入框
void AdapterBox_ShowEdkPrompt(int iIndex, int iId, bool (*pFun)(bool, int, yl::string &));

// 刷新softkey
void AdapterBox_UpdateSoftKey();

// 刷新softkey
void AdapterBox_InsertSoftKey(YLList<SoftKey_Data> & listSoftKey);

// 顶层界面是否为Tip
BOOL AdapterBox_IsTopTipUI();

// 顶层界面是否为MessageBox
BOOL AdapterBox_IsTopMessageBoxUI();

// enter zero interface
bool AdapterBox_ZeroOpenDialog(const yl::string & strPageAction, bool bRepaint = false);
// enter rps dialog
bool AdapterBox_ZeroEnterRPS();

// hide notifycenter
void AdapterBox_HideNotifyCenter();

// 是否处于设置向导中
BOOL AdapterBox_IsIsInWizard();

// 显示autop成功或失败界面
void AdapterBox_ShowAutopResult(bool bSuccess);

// 进入 RPS 界面
void AdapterBox_EnterRPS();

// ZERO
void AdapterBox_ZeroOpenDlg(const int iType);

//pop box
void AdapterBox_CreateMessageBox(void * pLister,
                                 const char * cszTitle,
                                 const char * cszContent,
                                 int eBtnType,
                                 unsigned int uExitTime = 0,
                                 int iTrigerType = -1,
                                 int eRegionType = 0);

void AdapterBox_CreateMenuTips(void * pListener,
                               int eTipsType,
                               const char * cszContent1,
                               const char * cszContent2,
                               unsigned int uExitTime,
                               int iTrigerType = -1);

void AdapterBox_ExitTriggers(void * pListener, int iTriggerType, BOOL bUpdate = TRUE);

bool AdapterBox_EnterMenu(const yl::string & strMenuID, int iIndex = 0);

bool AdapterBox_ExecuteSoftkey(const yl::string & strSoftkeyID);

void AdapterBox_ShowVolumeBar(bool bEnable);

// Exit ScreenSaver
void AdapterBox_ExitScreenSaver();

bool AdapterBox_ZeroAuthenticationAgain();

// Show Dsskey Lock Password
void AdapterBox_ShowDsskeyLockPassword(int iDsskeyID);

// Exit DlgDsskey
void AdapterBox_ExitDsskeyDlg();

int AdapterBox_ImageScale(const yl::string & strSrcFile, const yl::string & strDstFile,
                          int eImagetype, int nScaleXDef = 0, int nScaleYDef = 0);
// enter conf with
void AdapterBox_EnterConfWith(int iSessionID);

int  AdapterBox_GetGetTotalUnReadMessage();

// Dsskey long press
bool AdapterBox_DsskeyLongPress(int iDsskeyID);

#endif // __MOD_ADAPTERBOX_H__
