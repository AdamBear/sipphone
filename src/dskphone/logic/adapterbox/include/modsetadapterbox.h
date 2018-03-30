#ifndef __MOD_SET_ADAPTERBOX_H__
#define __MOD_SET_ADAPTERBOX_H__

#include "adapterboxdefine.h"

void AdapterBox_SetFunUploadConfig(FunUploadConfig fun);

void AdapterBox_SetFunUpdateFirmware(FunUpdateFirmware fun);

////////////////// UIManager begin
// 设置返回Idle函数
void AdapterBox_SetReturnToIdle(FunReturnToIdle fun);
// 设置更新界面函数
void AdapterBox_SetUpdateTopWnd(FunUpdateTopWnd fun);
////////////////// UIManager end

////////////////// InputHook begin
// 设置模拟硬按键函数
void AdapterBox_SetSendKey(FunSendKey fun);
////////////////// InputHook end

////////////////// talkui begin
// 清空话路数据
void AdapterBox_SetTalkUIClearSessionData(FunTalkUIClearSessionData fun);
// 话路退出
void AdapterBox_SetTalkUISessionExit(FunTalkUISessionExit fun);

// 获取通话时长
void AdapterBox_SetTalkUIGetTalkTimeBySessionId(FunTalkUIGetTalkTimeBySessionId fun);
// 获取输入内容
void AdapterBox_SetTalkUIGetInputContent(FunTalkUIGetInputContent fun);
// 获取不包括预输入的内容
void AdapterBox_SetTalkUIGetAlreadyInputContent(FunTalkUIGetAlreadyInputContent fun);
// 获取输入内容
void AdapterBox_SetTalkUIGetInputContentEx(FunTalkUIGetInputContentEx fun);
// 更新talkui
void AdapterBox_SetTalkUIUpdateWnd(FunTalkUIUpdateWnd fun);
// 更新talkui错误信息
void AdapterBox_SetTalkUIUpdateErrorInfo(FunTalkUIUpdateErrorInfo fun);
void AdapterBox_SetTalkUIIsInTalk(FunTalkUIIsInTalk fun);

// 获取输入内容
void AdapterBox_SetTalkUIGetInputCallID(FunTalkUIGetInputCallID fun);
////////////////// talkui end

// 进入SettingUI界面(ACD用)
void AdapterBox_SetSettingUIEnterPage(FunSettingUIEnterPage fun);

//设置进入联系人UI界面的函数指针
void AdapterBox_SetDirUIEnterLocalMenuPage(FunDirUIEnterLocalMenuPage fun);

// 判断是否测试模式
void AdapterBox_SetIsTestMode(FunIsTestMode fun);

// 进入测试模式
void AdapterBox_SetEnterTestMode(FunEnterTestMode fun);

// 退出测试模式
void AdapterBox_SetExitTestMode(FunExitTestMode fun);

// xmlBroser弹窗提示
void AdapterBox_SetXmlBrowserShowMessage(FunXmlBrowserShowMessage fun);

#if IF_FEATURE_XMLBROWSER_HOTELING
void AdapterBox_SetXmlBrowserShowTips(FunXmlBrowserShowTips fun);
#endif

// xmlBroser销毁弹窗
void AdapterBox_SetXmlBrowserDestroyMessage(FunXmlBrowserDestroyMessage fun);

// xmlBroser刷新UI界面
void AdapterBox_SetXmlBrowserUpdateWnd(FunXmlBrowserUpdateWnd fun);

// xmlBroser退出某界面
void AdapterBox_SetXmlBrowserExitXBDlg(FunXmlBrowserExitXBDlg fun);

// xmlBroser点灯
void AdapterBox_SetXmlBrowserCtrlLight(FunXmlBrowserCtrlLight fun);


// 显示sca菜单
void AdapterBox_SetShowSCAMenu(FunShowSCAMenu fun);

// 当前是否已显示SCA菜单
void AdapterBox_SetIsExistSCAMenu(FunIsExistSCAMenu fun);

// 退出SCA菜单
void AdapterBox_SetExitSCAMenu(FunExitSCAMenu fun);

#if IF_FEATURE_SHOW_BLF_CALLID
// 显示blf菜单
void AdapterBox_SetShowBLFMenu(FunShowBLFMenu fun);

// 当前是否已显示blf菜单
void AdapterBox_SetIsExistBLFMenu(FunIsExistBLFMenu fun);

// 退出blf菜单
void AdapterBox_SetExitBLFMenu(FunExitBLFMenu fun);
#endif

void AdapterBox_SetExitPickupMenu(FunExitPickupMenu fun);
// 进入SettingUI界面
void AdapterBox_SetEnterConnectVoiceMailPage(FunEnterConnectVoiceMailPage fun);

// 重新设置输入法
void AdapterBox_SetReloadIME(FunReloadIME fun);

// 重启话机
void AdapterBox_SetReboot(FunReboot fun);

// 恢复出厂
void AdapterBox_SetResetFactory(FunResetFactory fun);

//
void AdapterBox_SetXmlBrowserUpdate(FunXmlBrowserUpdate fun);

void AdapterBox_SetIsInIdle(FunIsInIdle fun);

void AdapterBox_SetVoiceMailCallout(FunVoiceMailCallOut fun);

void AdapterBox_SetVoiceMailGetUnReadNum(FunVoiceMailGetUnReadNum fun);

void AdapterBox_SetEnterCameraPage(FunEnterCameraPage fun);

void AdapterBox_SetIsInTalkUIFrame(FunIsInTalkUIFrame fun);

// 设置视频Mute图片 接口函数
void AdapterBox_SetMutePicture(FunGetMutePicture fun);

void AdapterBox_SetIsInTalk(FunIsInTalk fun);

void AdapterBox_SetMutePicture(FunGetMutePicture fun);
void AdapterBox_SetShowEdkPrompt(FunShowEdkPrompt fun);

void AdapterBox_SetUpdateSoftKey(FunUpdateSoftKey fun);

void AdapterBox_SetInsertSoftKey(FunInsertSoftKey fun);

void AdapterBox_SetIsTopTipUI(FunIsTopTipUI fun);

void AdapterBox_SetIsTopMessageBoxUI(FunIsTopMessageBoxUI fun);

// enter zero dialog
void AdapterBox_SetZeroOpenDialog(FunZeroOpenDialog fun);
// enter rps
void AdapterBox_SetZeroEnterRPS(FunZeroEnterPRS fun);

void AdapterBox_SetHideNotifyCenter(FunHideNotifyCenter fun);

void AdapterBox_SetIsInWizard(FunIsInWizard fun);

void AdapterBox_SetShowAutopResult(FunShowAutopResult fun);

void AdapterBox_SetEnterRPS(FunEnterRPS fun);

void AdapterBox_SetZEROOpenDlg(FunZEROOpenDlg fun);

void AdapterBox_SetCreateMessageBox(FunCreateMessageBox fun);

void AdapterBox_SetCreateMenuTips(FunCreateMenuTips fun);

void AdapterBox_SetExitTriggers(FunExitTriggers fun);

// 进入指定界面
void AdapterBox_SetEnterMenu(FunEnterMenu fun);

// 执行指定的softkey
void AdapterBox_SetExecuteSoftkey(FunExecuteSoftkey fun);

// 设置声音和音量条
void AdapterBox_SetShowVolumeBar(FunShowVolumeBar fun);

void AdapterBox_SetExitScreenSaver(FunExitScreenSaver fun);

// zero autop 再次鉴权
void AdapterBox_SetZeroAuthenticationAgain(FunZeroAuthenticationAgain fun);

void AdapterBox_SetShowDsskeyLockPassword(FunShowDsskeyLockPassword fun);
void AdapterBox_SetExitDsskeyDlg(FunExitDsskeyDlg fun);

void AdapterBox_SetImageScale(FunImageScale fun);

// 进入ConfWith界面
void AdapterBox_SetTalkUIEnterConfWith(FunTalkUIEnterConfWith fun);

void AdapterBox_SetGetTotalUnReadMessage(FunGetTotalUnReadMessage fun);

void AdapterBox_SetDsskeyLongPress(FunDsskeyLongPress fun);

#endif // __MOD_SET_ADAPTERBOX_H__
