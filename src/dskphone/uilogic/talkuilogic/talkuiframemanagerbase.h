#ifndef _TALKUI_FRAME_MANAGER_BASE_H
#define _TALKUI_FRAME_MANAGER_BASE_H

#include "uilogicbasetalkui.h"

#ifndef TALK_MSGBOX_SHORT_TIME
#define TALK_MSGBOX_SHORT_TIME           2000
#endif

class CTalkUIFrameManagerBase
{
public:
    CTalkUIFrameManagerBase(void);
    virtual ~CTalkUIFrameManagerBase(void);

public:
    //根据通话子窗口和数据 刷新UI
    //这边pBaseTalkUI 可为NULL， 表示退出当前通话UI显示  ， 但是不表示 退出通话框架  , 暂时是不会有对话框为NULL的情况
    virtual void UpdateUI(CUILogicBaseTalkUI * pUILogicBaseTalkUI,
                          CUILogicToTalkUIData & dataUILogicToTalkUI) = 0;

    //清除子窗口  UILogic 收到sessionID为 -1 的时候 会调用该函数
    virtual void ClearTalkUI() = 0;

    virtual void ClearTalkUICache() = 0;

    //创建通话UI
    virtual CUILogicBaseTalkUI * CreateTalkUI(const yl::string & strName) = 0;

    //释放通话UI
    virtual void ReleaseTalkUI(const yl::string & strProcessorName, CUILogicBaseTalkUI * pTalkUI) = 0;

    //提示错误信息 短暂提示如usb插入的相关提示
    virtual void UpdateOtherNotify(const yl::string strOtherNotify) = 0;

    //显示/刷新 错误信息  这是普通的弹出错误信息 如g722 等错误的信息, 是用弹框的形式出现的
    virtual void UpdateErrorInfo(const yl::string& strErrorTip, int nTime = TALK_MSGBOX_SHORT_TIME) = 0;

    //显示弹框提示
    virtual void UpdateMsgboxNoteNotify(const yl::string strMsgboxNoteNotify, int nTime = TALK_MSGBOX_SHORT_TIME) = 0;

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //更新ACD队列状态
    virtual void UpdateACDQueueStatus(yl::string strACDQueueStatus) = 0;
#endif

    //更新网络连接状态
    virtual void UpdateNetworkStatus(TALK_NETWORK_STATUS eStatus) = 0;

    //更新WIFI信号状态  bIsReady = TRUE 连接正常   bIsReady = FALSE 连接不正常
    virtual void UpdateWifiStatus(bool bIsReady) = 0;

    //把焦点的session数据设置无效
    //clearsessiondata 和 sessionExit 的是当前的焦点对话框的时候会调用这个函数
    virtual void SetFocusSessionDataInvalid() = 0;

    //更新安全等级信息
    virtual void UpdateSecurityClassification(bool bShowSecurity) = 0;

    virtual void UpdateVolumebar(bool bShow) = 0;

#if IF_TALKUI_USB_ENABLE
    //处理usb消息 连接状态改变
    //iState USB状态
    virtual LRESULT OnUSBRecordMsgStateChange(int iState) = 0;
    //录制警告
    // waparm： 警告值（单位分钟）
    virtual LRESULT OnUSBRecordMsgWarning(int iTime) = 0;
    //容量满 提醒
    virtual LRESULT OnUSBRecordMsgNotenough() = 0;
    //录制截图 提醒
    virtual LRESULT OnRecordStorageRecordResult(TALK_RECORD_STATUS eStatus) = 0;

#endif

    //刷新通道图标
    virtual void UpdateChannelIcon(TALKUI_CHANNEL_ICON eTalkUIChannelIcon) = 0;

    //刷新title
    virtual void UpdateTalkUITitle(const yl::string strTitle) = 0;

    //刷新pagenum显示
    //iCurrentIndex是实际的序号 开始序号会在iCurrentIndex的基础上加1
    virtual void UpdateTalkUIPageNum(int iCurrentIndex, int iSessionCount) = 0;

    //刷新通话时间显示
    virtual void UpdateTalkUITime(uint iHour, uint iMin, uint iSecond) = 0;

    //刷新softkey
    virtual void UpdateTalkUISoftkey() = 0;

    //刷新蓝牙图标
    virtual void UpdateBluetoothIcon(TALKUI_BLUETOOTH_ICON eTalkUIBlueToothIcon) = 0;

    //刷新通话图标
    //暂时是只有来电界面使用到
    //一般在实现这个接口的时候 最好是加上刷新蓝牙图标， 因为一般的中性版本普通图标和蓝牙图标是在一块显示的   这边容易把蓝牙图标也清除了
    virtual void UpdateTalkUIIcons() = 0;

public:

#if IF_TALKUI_CONTACT_PHOTO
    //获取会议联系人头像
    virtual yl::string GetPicContactsConference() = 0;

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //获取蓝牙联系人头像
    virtual yl::string GetPicBluetoothContact() = 0;
#endif

    virtual yl::string GetPicCloudContact() = 0;
#endif


};

#endif
