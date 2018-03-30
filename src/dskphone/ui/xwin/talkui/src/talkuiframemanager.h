#ifndef __TALKUI_FRAME_MANAGER_H__
#define __TALKUI_FRAME_MANAGER_H__

#include <ylstring.h>
#include "talkuilogic/talkuilogic_inc.h"
#include "dlgbasetalkui.h"
#include "dlgtalkuiframe.h"
#include "talkui_define.h"

#include "talkuilogic/talkuiframemanagerbase.h"
#include "talkuilogic/talkuilogiccommon.h"

namespace talkui
{

//当前通话框架类型
enum FRAME_TYPE
{
    FRAME_TYPE_NONE = 0,
    FRAME_TYPE_TALKUIFRAME,
    //FRAME_TYPE_CALLMANAGER,
};

// TalkUI的对外窗口
class CTalkUIFrameManager : public CTalkUIFrameManagerBase
{
public:
    static CTalkUIFrameManager & GetInstance();

protected:
    CTalkUIFrameManager();

public:
    virtual ~CTalkUIFrameManager();


public:
    //初始化
    bool Init();
    //反初始化
    void Unint();

public:
    //根据通话子窗口和数据 刷新UI
    //这边pBaseTalkUI 可为NULL， 表示退出当前通话UI显示  ， 但是不表示 退出通话框架  , 暂时是不会有对话框为NULL的情况
    virtual void UpdateUI(CUILogicBaseTalkUI * pUILogicBaseTalkUI,
                          CUILogicToTalkUIData & dataUILogicToTalkUI);

    //实际处理 切换显示 通话UI
    //这边pBaseTalkUI 可为NULL， 表示退出当前通话UI显示  ， 但是不表示 退出通话框架  , 暂时是不会有对话框为NULL的情况
    void OnUpdateUI(CUILogicBaseTalkUI * pUILogicBaseTalkUI,
                    CUILogicToTalkUIData & dataUILogicToTalkUI);

    //清除ui窗口缓存
    virtual void ClearTalkUICache();

    //把焦点的session数据设置无效
    //clearsessiondata 和 sessionExit 的是当前的焦点对话框的时候会调用这个函数
    virtual void SetFocusSessionDataInvalid();
    //清除子窗口  UILogic 收到sessionID为 -1 的时候 会调用该函数
    virtual void ClearTalkUI();

    //显示/刷新 错误信息  这是普通的弹出错误信息 如g722 等错误的信息, 是用弹框的形式出现的
    virtual void UpdateErrorInfo(const yl::string& strErrorTip, int nTime = TALK_MSGBOX_SHORT_TIME);

    //error note，一般是bsft录音使用
    virtual bool ShowErrorNote(CCallInfo * pCallInfo);
    virtual bool ShowErrorNote(CALL_RECORD_ACTION_RESULT eFailResult);

    //显示弹框提示
    virtual void UpdateMsgboxNoteNotify(const yl::string strMsgboxNoteNotify, int nTime = TALK_MSGBOX_SHORT_TIME);

    //显示 提示信息， 这个是在talkuiframe 的 标题栏 中显示的, 在拨号界面不会显示
    virtual void UpdateOtherNotify(yl::string strOtherNotify);

    ////显示talking 界面失败提示
    //void UpdateTalkingFailedNotify(CDlgBaseTalkUI* pTalkUI, yl::string strTalkingFailedNotify);


    // 获取子窗口指针
    CDlgBaseTalkUI * GetTalkUI() const;

public:
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //更新ACD队列状态
    virtual void UpdateACDQueueStatus(yl::string strACDQueueStatus);
#endif

    //更新网络连接状态  bIsReady = TRUE 连接正常   bIsReady = FALSE 连接不正常
    virtual void UpdateNetworkStatus(TALK_NETWORK_STATUS eStatus);

    //更新WIFI信号状态  bIsReady = TRUE 连接正常   bIsReady = FALSE 连接不正常
    virtual void UpdateWifiStatus(bool bIsReady);

    //更新安全等级信息
    virtual void UpdateSecurityClassification(bool bShowSecurity);

#if IF_TALKUI_USB_ENABLE
    //处理usb消息 连接状态改变
    //iState USB状态
    virtual LRESULT OnUSBRecordMsgStateChange(int iState);
    //录制警告
    // waparm： 警告值（单位分钟）
    virtual LRESULT OnUSBRecordMsgWarning(int iTime);
    //容量满 提醒
    virtual     LRESULT OnUSBRecordMsgNotenough();
    //录制截图 提醒
    //waparm：0=开始录制,1=停止录制,2=截图  lparam：成功与否(0/1)
    virtual LRESULT OnRecordStorageRecordResult(TALK_RECORD_STATUS eStatus);
#endif

    //刷新通道图标
    virtual void UpdateChannelIcon(TALKUI_CHANNEL_ICON eTalkUIChannelIcon);

    //刷新title
    virtual void UpdateTalkUITitle(yl::string strTitle);

    //刷新pagenum显示
    //iCurrentIndex是实际的序号  开始序号会在iCurrentIndex的基础上加1
    virtual void UpdateTalkUIPageNum(int iCurrentIndex, int iSessionCount);

    //刷新通话时间显示
    virtual void UpdateTalkUITime(uint iHour, uint iMin, uint iSecond);

    //刷新softkey
    virtual void UpdateTalkUISoftkey();

    //刷新蓝牙图标
    virtual void UpdateBluetoothIcon(TALKUI_BLUETOOTH_ICON eTalkUIBlueToothIcon);

    //刷新通话图标
    //暂时是只有来电界面使用到
    //一般在实现这个接口的时候 最好是加上刷新蓝牙图标， 因为一般的中性版本普通图标和蓝牙图标是在一块显示的   这边容易把蓝牙图标也清除了
    virtual void UpdateTalkUIIcons();

    virtual void UpdateVolumebar(bool bShow);

#if IF_TALKUI_CONTACT_PHOTO
public:
    //获取会议联系人头像
    virtual yl::string GetPicContactsConference();

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //获取蓝牙联系人头像
    virtual yl::string GetPicBluetoothContact();
#endif

    virtual yl::string GetPicCloudContact()
    {
        return "";
    }
#endif

protected:
    //创建TalkUIFrame实例
    void CreateTalkUIFrame();

public:
    //创建通话UI实例
    virtual CUILogicBaseTalkUI * CreateTalkUI(const yl::string & strName);
    //释放通话UI实例指针
    virtual void ReleaseTalkUI(const yl::string & strProcessorName, CUILogicBaseTalkUI * pTalkUI);

public:
    void SetCurrentFrameType(FRAME_TYPE frameType);
    FRAME_TYPE GetCurrentFrameType() const;

protected:
    //保存活动的Frame框架指针
    FRAME_TYPE m_frameType;

protected:
    CDlgTalkUIFrame * m_pTalkUIFrame;       //通话框架指针
    bool m_bIsTalkUIFrameNew;           //talkuiframe指针是否是新new出来的  主要是用来做settopwnd的时候使用

};



#define _TalkUIFrameManager CTalkUIFrameManager::GetInstance()

}       //namespace talkui

#endif // __TALKUI_FRAME_H__
