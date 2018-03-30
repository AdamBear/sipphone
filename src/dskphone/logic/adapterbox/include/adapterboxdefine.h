#ifndef __ADAPTERBOX_DEFINE_H__
#define __ADAPTERBOX_DEFINE_H__

#include "ylstring.h"
#include <ETLLib.hpp>

struct SoftKey_Data;
////////////////// UIManager begin
// 返回Idle界面的函数定义
typedef void (*FunReturnToIdle)(bool bClickToIdle);
// 更新界面的函数定义
typedef void (*FunUpdateTopWnd)();
////////////////// UIManager end

////////////////// InputHook begin
// 模拟硬按键
typedef void (*FunSendKey)(int nKeyCode, bool bPress);
////////////////// InputHook end

////////////////// talkui begin
// 清空话路数据
typedef void (*FunTalkUIClearSessionData)(int iSessionID); // talkui_ClearSessionData
// 通知ui话路退出
typedef void (*FunTalkUISessionExit)(int iSessionID); // talkui_SessionExit
// 获取通话时长
typedef int (*FunTalkUIGetTalkTimeBySessionId)(int iSessionID);  // talkui_GetTalkTimeBySessionId
// 获取输入内容
typedef yl::string(*FunTalkUIGetInputContent)();  // talkui_GetInputContent
// 获取已经输入内容
typedef yl::string(*FunTalkUIGetAlreadyInputContent)();
// 获取输入内容
typedef bool (*FunTalkUIGetInputContentEx)(yl::string & strNum,
        yl::string & strDisplayname); // talkui_GetInputContent
// 刷新通话ui
typedef void (*FunTalkUIUpdateWnd)(void * pData); // talkui_UpdateWnd
// 更新通话错误信息
typedef void (*FunTalkUIUpdateErrorInfo)(void * pData); // talkui_UpdateErrorInfo:t4x
typedef bool (*FunTalkUIIsInTalk)(); // talkui_UpdateErrorInfo:t4x
// 获取输入内容的CALL ID
typedef int (*FunTalkUIGetInputCallID)(); // talkui_GetInputCallID

////////////////// talkui end

// 进入SettingUI界面（ACD用）
typedef bool (*FunSettingUIEnterPage)(const int ePageType, int iFocusIndex);

//进入联系人UI界面
typedef BOOL (*FunDirUIEnterLocalMenuPage)();

////////////////// testmode begin
// 判断是否测试模式
typedef bool (*FunIsTestMode)();

// 进入测试模式
typedef void (*FunEnterTestMode)(int iTestType);

// 退出测试模式
typedef void (*FunExitTestMode)();

////////////////// testmode end

// xmlBrowser ShowMessage弹出提示框
typedef void (*FunXmlBrowserShowMessage)(int iTipType, UINT uTimeout);
#if IF_FEATURE_XMLBROWSER_HOTELING
typedef void (*FunXmlBrowserShowTips)(const yl::string & strMessage, UINT uTimeout);
#endif

// xmlBrowser 退出提示框
typedef void (*FunXmlBrowserDestroyMessage)();

// xmlBrowser 刷新界面
typedef void (*FunXmlBrowserUpdateWnd)();

// xmlBrowser 刷新界面
typedef void (*FunXmlBrowserExitXBDlg)(int iIndex, bool bUpdate);

// xmlBrowser 点灯
typedef bool (*FunXmlBrowserCtrlLight)(int iDsskeyIndex, int iColorType, int iFlushType);


// 显示sca菜单
typedef bool (*FunShowSCAMenu)(void * pData);

// 当前是否已显示SCA菜单
typedef bool (*FunIsExistSCAMenu)();

// 退出SCA菜单
typedef void (*FunExitSCAMenu)();

#if IF_FEATURE_SHOW_BLF_CALLID
// 显示BLF菜单
typedef bool(*FunShowBLFMenu)(void *pData);

// 当前是否已显示BLF菜单
typedef bool(*FunIsExistBLFMenu)(const yl::string& strKey);

// 退出BLF菜单
typedef void(*FunExitBLFMenu)();
#endif

typedef void (*FunExitPickupMenu)();
// 进入连接Voicemail界面
typedef bool (*FunEnterConnectVoiceMailPage)(int iAccount);

// 重新加载输入法
typedef void (*FunReloadIME)();

// 重启话机
typedef void (*FunReboot)(int iTimeDelay);

// 恢复出厂
typedef void (*FunResetFactory)(int iResetType);

// 更新XML数据
typedef void (*FunXmlBrowserUpdate)();

// 是否处于Idle界面
typedef bool (*FunIsInIdle)();

typedef bool(*FunVoiceMailCallOut)(int iIndex); //SettingUI_VoiceMailCallOut

typedef int(*FunVoiceMailGetUnReadNum)(int iAccountID, bool bNotify); //vmail_GetUnReadVoiceMailNum
typedef bool (*FunEnterCameraPage)();
typedef bool (*FunIsInTalkUIFrame)();
// 获取视频Mute图片
typedef yl::string(*FunGetMutePicture)(int iResolutionType);

// 是否处于talk界面
typedef bool (*FunIsInTalk)();

// efk弹出框
typedef void (*FunShowEdkPrompt)(int iIndex, int iId, bool (*pFun)(bool, int, yl::string &));

// 刷新softkey
typedef void (*FunUpdateSoftKey)();

typedef void (*FunInsertSoftKey)(YLList<SoftKey_Data> &);

typedef BOOL (*FunIsTopTipUI)();

typedef BOOL (*FunIsTopMessageBoxUI)();

// enter zero dialog
typedef bool (*FunZeroOpenDialog)(const yl::string & strPageAction, bool bRepaint);
// enter rps dialog
typedef bool (*FunZeroEnterPRS)();

// hide notifycenter
typedef void (*FunHideNotifyCenter)();

typedef BOOL (*FunIsInWizard)();

typedef BOOL (*FunShowAutopResult)(bool bSuccess);

typedef void (*FunEnterRPS)();

typedef bool (*FunZEROOpenDlg)(int iType);

typedef void (*FunCreateMessageBox)(void * pLister,
                                    const char * cszTitle,
                                    const char * cszContent,
                                    int eBtnType,
                                    unsigned int uExitTime,
                                    int iTrigerType,
                                    int eRegionType);

typedef void (*FunCreateMenuTips)(void * pListener,
                                  int eTipsType,
                                  const char * cszContent1,
                                  const char * cszContent2,
                                  unsigned int uExitTime,
                                  int iTrigerType);

typedef void (*FunExitTriggers)(void * pListener, int iTriggerType, BOOL bUpdate);

// 进入指定的menu
typedef bool (*FunEnterMenu)(const yl::string & strMenuID, int iIndex);

// 执行softkey
typedef bool(*FunExecuteSoftkey)(const yl::string & strSoftkeyID);

// 设置音量,显示音量条
typedef void (*FunShowVolumeBar)(bool bEnable);

typedef void (*FunExitScreenSaver)();

typedef bool (*FunZeroAuthenticationAgain)();

typedef void (*FunShowDsskeyLockPassword)(int iDssKeyID);

typedef void (*FunExitDsskeyDlg)();

typedef int(*FunImageScale)(const yl::string & strSrcFile, const yl::string & strDstFile,
                            int eImagetype, int nScaleXDef, int nScaleYDef);
// 通知ui进入Conf with界面
typedef void(*FunTalkUIEnterConfWith)(int iSessionID); // talkui_EnterConfWith

typedef void(*FunUpdateFirmware)();

typedef void(*FunUploadConfig)();

typedef int(*FunGetTotalUnReadMessage)();

typedef bool (*FunDsskeyLongPress)(int iDssKeyID);

#endif // __ADAPTERBOX_DEFINE_H__
