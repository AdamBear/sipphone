#include "pageuiconditioncontroller.h"
#include "account/include/modaccount.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "settingui/include/itemnamedefine.h"
#include "contacts/directory/include/moddirinterface.h"
#include "diruilogic/include/directorycommon.h"
#include "contacts/ldaplogic/include/modldap.h"
#include "configparser/modconfigparser.h"
#include "feature/include/modfeature.h"
#include "edkui/include/modedkui.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "record/include/modrecord.h"
#include "settingui/include/pageiddefine.h"
#include "wireless/wifi/include/modwifi.h"

// 界面条件初始化, 加载page id/dir type和函数指针
void PageAllowConditionInit()
{

#if IF_FEATURE_EDK
    // setting
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDUserMode);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDCallWaiting);

    // setting + broadsoft
//  EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDFeaturesDND);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDPhoneDND);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDDNDLineSelect);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszEdkPageIDCustomAccountDND);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDCallForward);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDAlwaysForward);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDBusyForward);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDNoAnswerForward);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDCallForwardLineSelect);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszEdkPageIDCustomAccountForward);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszEdkPageIDCustomAlwaysForward);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszEdkPageIDCustomBusyForward);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszEdkPageIDCustomNoAnswerForward);
#ifdef IF_FEATURE_PSTN
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDPSTNCountry);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDPSTNFlashTime);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDPSTNAccountList);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDAdvPSTNAccountSetting);
#endif // IF_FEATURE_PSTN
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDUsbIdleRecord);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDUsbAudioList);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDUsbStorageInfo);
#endif

#ifdef IF_SUPPORT_WIFI
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDWifiScan);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDWifiKnown);
#endif

    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDStatusNetworkIPv4);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDStatusNetworkIPv6);
    EdkUI_SetAllowEnterUIFunc(IsPageSettingAllow, kszPageIDStatusRTP);

    // broadsoft
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDBSFTCallControll);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDAnyWhereLocationList);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllRemoteOffice);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllHideNumber);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllAnonymoRrejection);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllSimultaneousRingList);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllCallWaiting);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDSecurityClassfication);
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllInitialACDStatus);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllSetACDStatus);
#endif
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllMobility);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllGNF);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllForwardSelective);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllExecutiveDetails);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllExecutiveCommon);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllExecutiveFilter);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllExecutiveScreen);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllExecutiveAlter);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllExecutiveListview);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllAssistantDetails);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllAssistantCommon);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftAllow, kszPageIDCallControllAssistantListview);

    EdkUI_SetAllowEnterUIFunc(IsPageBsftSelectLineAllow, kszEdkPageIDCallcontrolAnywhereSelectline);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftSelectLineAllow, kszEdkPageIDCallcontrolRemoteOfficeSelectline);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftSelectLineAllow, kszEdkPageIDCallcontrolAnonymousCallselectline);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftSelectLineAllow, kszEdkPageIDCallcontrolAnonymousRejectionselectline);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftSelectLineAllow, kszEdkPageIDCallcontrolSimultaneousRingSelectline);
#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
    EdkUI_SetAllowEnterUIFunc(IsPageBsftSelectLineAllow, kszEdkPageIDCallcontrolCallWaitingselectline);
#endif
    EdkUI_SetAllowEnterUIFunc(IsPageBsftSelectLineAllow, kszEdkPageIDCallcontrolMobilityselectline);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftSelectLineAllow, kszEdkPageIDCallcontrolGroupNightForwardSelectline);
    EdkUI_SetAllowEnterUIFunc(IsPageBsftSelectLineAllow, kszEdkPageIDCallcontrolforwardselectiveselectline);
#endif // IF_FEATURE_EDK
}

// 是否允许进入bsft call control 判断
bool IsPageBsftAllow(const yl::string & strPageName/* = ""*/)
{
    bool bIsAllow = false;

    // 进入call control的开关
    if (configParser_GetConfigInt(kszBroadsoftActive) == 1
            && configParser_GetConfigInt(kszXSISwitch) == 1)
    {
        bIsAllow = true;
    }

    // 默认账号xsi信息判断
    if (kszPageIDAnyWhereLocationList == strPageName
            || kszPageIDCallControllRemoteOffice == strPageName
            || kszPageIDCallControllHideNumber == strPageName
            || kszPageIDCallControllAnonymoRrejection == strPageName
            || kszPageIDCallControllSimultaneousRingList == strPageName
            || kszPageIDCallControllCallWaiting == strPageName
            || kszPageIDCallControllMobility == strPageName
            || kszPageIDCallControllGNF == strPageName
            || kszPageIDCallControllForwardSelective == strPageName
       )
    {
        // 注册上的账号>1, 则不让进入(用户不知道当前进的是哪个line的配置界面)
        int iLineID = acc_GetDefaultAccount();
        if (acc_GetRegisteredAccountNum() > 1
                || !BWCallControl_IsAccountXSIEnable(iLineID))
        {
            return false;
        }
    }

    if (strPageName == kszPageIDCallControllCallWaiting)
    {
        bIsAllow = bIsAllow && 1 == configParser_GetConfigInt(kszCallWaitingMode);
    }
    if (strPageName == kszPageIDCallControllInitialACDStatus
            || strPageName == kszPageIDCallControllSetACDStatus)
    {
        bIsAllow = bIsAllow && CallCenter_IsACDOn() && ACD_BROADSOFT == Get_ACDType();
    }

    return bIsAllow;
}

// 是否允许进入bsft call control 账号选择界面判断
bool IsPageBsftSelectLineAllow(const yl::string & strPageName/* = ""*/)
{

    // 可用账号数量为0, 账号选择界面不可进
    if (!IsPageBsftAllow()
            || acc_GetRegisteredAccountNum() < 1)
    {
        return false;
    }

    bool bIsAllow = true;
    if (strPageName == kszPageIDCallControllCallWaiting)
    {
        bIsAllow = IsPageBsftAllow(strPageName);
    }

    return bIsAllow;

}

// 是否允许进入setting界面判断
bool IsPageSettingAllow(const yl::string & strPageName /*= ""*/)
{

    bool bIsAllow = true;

    if (kszPageIDUserMode == strPageName)
    {
        bIsAllow = feature_UserModeIsOpen();
    }
    // features 下的call waiting与call control下的互斥, 后者优先级高
    else if (kszPageIDCallWaiting == strPageName)
    {
        bIsAllow = !IsPageBsftAllow(kszPageIDCallControllCallWaiting);
    }
    // ---- sharing setting and bsft
    else if (kszPageIDPhoneDND == strPageName)
    {
        // phone dnd与custom dnd互斥
        bIsAllow = (0 == configParser_GetConfigInt(kszDndMode));
    }
    else if (kszPageIDDNDLineSelect == strPageName
             || kszEdkPageIDCustomAccountDND == strPageName)
    {
        bIsAllow = (1 == configParser_GetConfigInt(kszDndMode));

        // 账号数量为1才允许进入账号的dnd设置界面
        if (bIsAllow && kszEdkPageIDCustomAccountDND == strPageName)
        {
            bIsAllow = (1 == acc_GetRegisteredAccountNum());
        }
    }
    else if (kszPageIDAlwaysForward == strPageName
             || kszPageIDBusyForward == strPageName
             || kszPageIDNoAnswerForward == strPageName
             || kszPageIDCallForward == strPageName)
    {
        // phone fwd与custom fwd互斥
        bIsAllow = (0 == configParser_GetConfigInt(kszForwardMode));
    }
    else if (kszPageIDCallForwardLineSelect == strPageName
             || kszEdkPageIDCustomAccountForward == strPageName
             || kszEdkPageIDCustomAlwaysForward == strPageName
             || kszEdkPageIDCustomBusyForward == strPageName
             || kszEdkPageIDCustomNoAnswerForward == strPageName)
    {
        bIsAllow = (1 == configParser_GetConfigInt(kszForwardMode));

        // 账号数量为1才允许进入账号的fwd设置界面
        if (bIsAllow && kszPageIDCallForwardLineSelect != strPageName)
        {
            bIsAllow = (1 == acc_GetRegisteredAccountNum());
        }
    }
#ifdef IF_FEATURE_PSTN
    else if (kszPageIDPSTNCountry == strPageName
             || kszPageIDPSTNFlashTime == strPageName)
    {
        // 插着PSTN Box设备才允许进入国家设置界面
        bIsAllow = (pstnadapter_GetDeviceCount() > 0);
    }
    else if (kszPageIDPSTNAccountList == strPageName
             || kszPageIDAdvPSTNAccountSetting == strPageName)
    {
        // 有PSTN账号才允许进入PSTN账号状态/设置界面
        bIsAllow = (acc_AccountNum(ACCOUNT_TYPE_PSTN) > 0);
    }
#endif // IF_FEATURE_PSTN
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    else if (kszPageIDUsbIdleRecord == strPageName)
    {
        // 进入本地录音界面的条件：接入U盘；启用USB录音功能；启用本地录音功能
        bIsAllow = modRecord_IsStorageConnect()
                   && modRecord_IsRecordEnable(RECORD_AUDIO)
                   && configParser_GetConfigInt(kszUSBIdleRecordEnable) == 1;
    }
    else if (kszPageIDUsbStorageInfo == strPageName
             || kszPageIDUsbAudioList == strPageName)
    {
        bIsAllow = modRecord_IsStorageConnect();
    }
#endif
#ifdef IF_SUPPORT_WIFI
    else if (kszPageIDWifiScan == strPageName
             || kszPageIDWifiKnown == strPageName)
    {
        // Wi-Fi开启才允许进入网络列表界面
        bIsAllow = WIFI_IsEnable();
    }
#endif
    else if (kszPageIDStatusNetworkIPv4 == strPageName)
    {
        bIsAllow = netIsOpenIPv4Mode();
    }
    else if (kszPageIDStatusNetworkIPv6 == strPageName)
    {
        bIsAllow = netIsOpenIPv6Mode();
    }
    else if (kszPageIDStatusRTP == strPageName)
    {
        bIsAllow = (1 == configParser_GetConfigInt(kszDisplayVQReportOnUI));
    }

    return bIsAllow;

}
