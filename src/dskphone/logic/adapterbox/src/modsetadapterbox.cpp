#include "modsetadapterbox.h"
#include "adapterbox.h"
#include <ETLLib.hpp>

#define ADAPTER_SET_FUN(FunType, fun)  (_AdapterBox.m_##FunType = fun)


void AdapterBox_SetFunUploadConfig(FunUploadConfig fun)
{
    ADAPTER_SET_FUN(FunUploadConfig, fun);
}

void AdapterBox_SetFunUpdateFirmware(FunUpdateFirmware fun)
{
    ADAPTER_SET_FUN(FunUpdateFirmware, fun);
}

// 设置返回Idle函数
void AdapterBox_SetReturnToIdle(FunReturnToIdle fun)
{
    ADAPTER_SET_FUN(FunReturnToIdle, fun);
}

// 设置更新界面函数
void AdapterBox_SetUpdateTopWnd(FunUpdateTopWnd fun)
{
    ADAPTER_SET_FUN(FunUpdateTopWnd, fun);
}

void AdapterBox_SetSendKey(FunSendKey fun)
{
    ADAPTER_SET_FUN(FunSendKey, fun);
}

// 清空话路数据
void AdapterBox_SetTalkUIClearSessionData(FunTalkUIClearSessionData fun)
{
    ADAPTER_SET_FUN(FunTalkUIClearSessionData, fun);
}

// 话路退出
void AdapterBox_SetTalkUISessionExit(FunTalkUISessionExit fun)
{
    ADAPTER_SET_FUN(FunTalkUISessionExit, fun);
}

// 获取通话时长
void AdapterBox_SetTalkUIGetTalkTimeBySessionId(FunTalkUIGetTalkTimeBySessionId fun)
{
    ADAPTER_SET_FUN(FunTalkUIGetTalkTimeBySessionId, fun);
}

// 获取输入内容
void AdapterBox_SetTalkUIGetInputContent(FunTalkUIGetInputContent fun)
{
    ADAPTER_SET_FUN(FunTalkUIGetInputContent, fun);
}

// 获取不包括预输入的内容
void AdapterBox_SetTalkUIGetAlreadyInputContent(FunTalkUIGetAlreadyInputContent fun)
{
    ADAPTER_SET_FUN(FunTalkUIGetAlreadyInputContent, fun);
}

// 获取输入内容
void AdapterBox_SetTalkUIGetInputContentEx(FunTalkUIGetInputContentEx fun)
{
    ADAPTER_SET_FUN(FunTalkUIGetInputContentEx, fun);
}

// 获取输入内容的CallID
void AdapterBox_SetTalkUIGetInputCallID(FunTalkUIGetInputCallID fun)
{
    ADAPTER_SET_FUN(FunTalkUIGetInputCallID, fun);
}

// 更新talkui
void AdapterBox_SetTalkUIUpdateWnd(FunTalkUIUpdateWnd fun)
{
    ADAPTER_SET_FUN(FunTalkUIUpdateWnd, fun);
}

// 更新talkui错误信息
void AdapterBox_SetTalkUIUpdateErrorInfo(FunTalkUIUpdateErrorInfo fun)
{
    ADAPTER_SET_FUN(FunTalkUIUpdateErrorInfo, fun);
}

// 当前是否是通话界面
void AdapterBox_SetTalkUIIsInTalk(FunTalkUIIsInTalk fun)
{
    ADAPTER_SET_FUN(FunTalkUIIsInTalk, fun);
}

// 进入SettingUI界面(ACD用)
void AdapterBox_SetSettingUIEnterPage(FunSettingUIEnterPage fun)
{
    ADAPTER_SET_FUN(FunSettingUIEnterPage, fun);
}

void AdapterBox_SetDirUIEnterLocalMenuPage(FunDirUIEnterLocalMenuPage fun)
{
    ADAPTER_SET_FUN(FunDirUIEnterLocalMenuPage, fun);
}

// 判断是否测试模式
void AdapterBox_SetIsTestMode(FunIsTestMode fun)
{
    ADAPTER_SET_FUN(FunIsTestMode, fun);
}

// 进入测试模式
void AdapterBox_SetEnterTestMode(FunEnterTestMode fun)
{
    ADAPTER_SET_FUN(FunEnterTestMode, fun);
}

// 退出测试模式
void AdapterBox_SetExitTestMode(FunExitTestMode fun)
{
    ADAPTER_SET_FUN(FunExitTestMode, fun);
}

// xmlBroser弹窗提示
void AdapterBox_SetXmlBrowserShowMessage(FunXmlBrowserShowMessage fun)
{
    ADAPTER_SET_FUN(FunXmlBrowserShowMessage, fun);
}

#if IF_FEATURE_XMLBROWSER_HOTELING
// xmlBroser弹窗提示
void AdapterBox_SetXmlBrowserShowTips(FunXmlBrowserShowTips fun)
{
    ADAPTER_SET_FUN(FunXmlBrowserShowTips, fun);
}
#endif

// xmlBroser销毁弹窗
void AdapterBox_SetXmlBrowserDestroyMessage(FunXmlBrowserDestroyMessage fun)
{
    ADAPTER_SET_FUN(FunXmlBrowserDestroyMessage, fun);
}

// xmlBroser刷新UI界面
void AdapterBox_SetXmlBrowserUpdateWnd(FunXmlBrowserUpdateWnd fun)
{
    ADAPTER_SET_FUN(FunXmlBrowserUpdateWnd, fun);
}

// xmlBroser退出某界面
void AdapterBox_SetXmlBrowserExitXBDlg(FunXmlBrowserExitXBDlg fun)
{
    ADAPTER_SET_FUN(FunXmlBrowserExitXBDlg, fun);
}

// xmlBroser点灯
void AdapterBox_SetXmlBrowserCtrlLight(FunXmlBrowserCtrlLight fun)
{
    ADAPTER_SET_FUN(FunXmlBrowserCtrlLight, fun);
}

// 显示sca菜单
void AdapterBox_SetShowSCAMenu(FunShowSCAMenu fun)
{
    ADAPTER_SET_FUN(FunShowSCAMenu, fun);
}

// 当前是否已显示SCA菜单
void AdapterBox_SetIsExistSCAMenu(FunIsExistSCAMenu fun)
{
    ADAPTER_SET_FUN(FunIsExistSCAMenu, fun);
}

// 退出SCA菜单
void AdapterBox_SetExitSCAMenu(FunExitSCAMenu fun)
{
    ADAPTER_SET_FUN(FunExitSCAMenu, fun);
}

#if IF_FEATURE_SHOW_BLF_CALLID
// 显示blf菜单
void AdapterBox_SetShowBLFMenu(FunShowBLFMenu fun)
{
    ADAPTER_SET_FUN(FunShowBLFMenu, fun);
}

// 当前是否已显示blf菜单
void AdapterBox_SetIsExistBLFMenu(FunIsExistBLFMenu fun)
{
    ADAPTER_SET_FUN(FunIsExistBLFMenu, fun);
}

// 退出blf菜单
void AdapterBox_SetExitBLFMenu(FunExitBLFMenu fun)
{
    ADAPTER_SET_FUN(FunExitBLFMenu, fun);
}
#endif

// 退出PickupMenu界面
void AdapterBox_SetExitPickupMenu(FunExitPickupMenu fun)
{
    ADAPTER_SET_FUN(FunExitPickupMenu, fun);
}

void AdapterBox_SetEnterConnectVoiceMailPage(FunEnterConnectVoiceMailPage fun)
{
    ADAPTER_SET_FUN(FunEnterConnectVoiceMailPage, fun);
}

void AdapterBox_SetReloadIME(FunReloadIME fun)
{
    ADAPTER_SET_FUN(FunReloadIME, fun);
}

void AdapterBox_SetReboot(FunReboot fun)
{
    ADAPTER_SET_FUN(FunReboot, fun);
}

void AdapterBox_SetResetFactory(FunResetFactory fun)
{
    ADAPTER_SET_FUN(FunResetFactory, fun);
}

void AdapterBox_SetXmlBrowserUpdate(FunXmlBrowserUpdate fun)
{
    ADAPTER_SET_FUN(FunXmlBrowserUpdate, fun);
}

void AdapterBox_SetIsInIdle(FunIsInIdle fun)
{
    ADAPTER_SET_FUN(FunIsInIdle, fun);
}

void AdapterBox_SetVoiceMailCallout(FunVoiceMailCallOut fun)
{
    ADAPTER_SET_FUN(FunVoiceMailCallOut, fun);
}

void AdapterBox_SetVoiceMailGetUnReadNum(FunVoiceMailGetUnReadNum fun)
{
    ADAPTER_SET_FUN(FunVoiceMailGetUnReadNum, fun);
}

void AdapterBox_SetEnterCameraPage(FunEnterCameraPage fun)
{
    ADAPTER_SET_FUN(FunEnterCameraPage, fun);
}
void AdapterBox_SetIsInTalkUIFrame(FunIsInTalkUIFrame fun)
{
    ADAPTER_SET_FUN(FunIsInTalkUIFrame, fun);
}
// 设置视频Mute图片 接口函数
void AdapterBox_SetMutePicture(FunGetMutePicture fun)
{
    ADAPTER_SET_FUN(FunGetMutePicture, fun);
}

void AdapterBox_SetShowEdkPrompt(FunShowEdkPrompt fun)
{
    ADAPTER_SET_FUN(FunShowEdkPrompt, fun);
}

void AdapterBox_SetUpdateSoftKey(FunUpdateSoftKey fun)
{
    ADAPTER_SET_FUN(FunUpdateSoftKey, fun);
}

void AdapterBox_SetIsInTalk(FunIsInTalk fun)
{
    ADAPTER_SET_FUN(FunIsInTalk, fun);
}

void AdapterBox_SetInsertSoftKey(FunInsertSoftKey fun)
{
    ADAPTER_SET_FUN(FunInsertSoftKey, fun);
}

void AdapterBox_SetIsTopTipUI(FunIsTopTipUI fun)
{
    ADAPTER_SET_FUN(FunIsTopTipUI, fun);
}

void AdapterBox_SetIsTopMessageBoxUI(FunIsTopMessageBoxUI fun)
{
    ADAPTER_SET_FUN(FunIsTopMessageBoxUI, fun);
}

void AdapterBox_SetIsInWizard(FunIsInWizard fun)
{
    ADAPTER_SET_FUN(FunIsInWizard, fun);
}

void AdapterBox_SetShowAutopResult(FunShowAutopResult fun)
{
    ADAPTER_SET_FUN(FunShowAutopResult, fun);
}

void AdapterBox_SetEnterRPS(FunEnterRPS fun)
{
    ADAPTER_SET_FUN(FunEnterRPS, fun);
}

void AdapterBox_SetZEROOpenDlg(FunZEROOpenDlg fun)
{
    ADAPTER_SET_FUN(FunZEROOpenDlg, fun);
}

void AdapterBox_SetCreateMessageBox(FunCreateMessageBox fun)
{
    ADAPTER_SET_FUN(FunCreateMessageBox, fun);
}

void AdapterBox_SetCreateMenuTips(FunCreateMenuTips fun)
{
    ADAPTER_SET_FUN(FunCreateMenuTips, fun);
}

void AdapterBox_SetExitTriggers(FunExitTriggers fun)
{
    ADAPTER_SET_FUN(FunExitTriggers, fun);
}

// enter zero dialog
void AdapterBox_SetZeroOpenDialog(FunZeroOpenDialog fun)
{
    ADAPTER_SET_FUN(FunZeroOpenDialog, fun);
}

// enter rps
void AdapterBox_SetZeroEnterRPS(FunZeroEnterPRS fun)
{
    ADAPTER_SET_FUN(FunZeroEnterPRS, fun);
}

// hide notifycenter
void AdapterBox_SetHideNotifyCenter(FunHideNotifyCenter fun)
{
    ADAPTER_SET_FUN(FunHideNotifyCenter, fun);
}

// 进入指定界面
void AdapterBox_SetEnterMenu(FunEnterMenu fun)
{
    ADAPTER_SET_FUN(FunEnterMenu, fun);
}

// 执行指定的softkey
void AdapterBox_SetExecuteSoftkey(FunExecuteSoftkey fun)
{
    ADAPTER_SET_FUN(FunExecuteSoftkey, fun);
}

void AdapterBox_SetShowVolumeBar(FunShowVolumeBar fun)
{
    ADAPTER_SET_FUN(FunShowVolumeBar, fun);
}

// exit screen saver
void AdapterBox_SetExitScreenSaver(FunExitScreenSaver fun)
{
    ADAPTER_SET_FUN(FunExitScreenSaver, fun);
}

// enter zero AuthenticationAgain
void AdapterBox_SetZeroAuthenticationAgain(FunZeroAuthenticationAgain fun)
{
    ADAPTER_SET_FUN(FunZeroAuthenticationAgain, fun);
}

void AdapterBox_SetShowDsskeyLockPassword(FunShowDsskeyLockPassword fun)
{
    ADAPTER_SET_FUN(FunShowDsskeyLockPassword, fun);
}


void AdapterBox_SetExitDsskeyDlg(FunExitDsskeyDlg fun)
{
    ADAPTER_SET_FUN(FunExitDsskeyDlg, fun);
}

void AdapterBox_SetImageScale(FunImageScale fun)
{
    ADAPTER_SET_FUN(FunImageScale, fun);
}

// 进入ConfWith界面
void AdapterBox_SetTalkUIEnterConfWith(FunTalkUIEnterConfWith fun)
{
    ADAPTER_SET_FUN(FunTalkUIEnterConfWith, fun);
}

void AdapterBox_SetGetTotalUnReadMessage(FunGetTotalUnReadMessage fun)
{
    ADAPTER_SET_FUN(FunGetTotalUnReadMessage, fun);
}

void AdapterBox_SetDsskeyLongPress(FunDsskeyLongPress fun)
{
    ADAPTER_SET_FUN(FunDsskeyLongPress, fun);
}
