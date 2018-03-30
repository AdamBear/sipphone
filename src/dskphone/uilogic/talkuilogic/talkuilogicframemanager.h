#ifndef __TALK_UILOGIC_FRAME_MANAGER_H__
#define __TALK_UILOGIC_FRAME_MANAGER_H__

#include <singletonclassdefine.h>
#include <ylstring.h>
#include "talkuilogic_inc.h"
#include "talkuiframemanagerbase.h"


class CTalkUILogicFrameManager
{
    DECLARE_SINGLETON_CLASS(CTalkUILogicFrameManager)

public:
    //初始化
    bool Init();
    //反初始化
    void Uninit();
    //根据逻辑层上传数据刷新UI
    bool UpdateUI(DataLogic2UI * pDataLogic2UI);

    //刷新当前窗口
    void UpdateCurrentWnd(UPDATE_REASON eReason = UPDATE_DEFAULT);


    //根据数据和名称更新窗口
//  bool UpdateUIByProcessorName(DataLogic2UI* pDataLogic2UI, yl::string& strProcessorName);
    bool UpdateUIByProcessorName(const yl::string & strProcessorName);

    //刷新当前界面
    void RefreshUI();

    //退出话路 状态改变
    void SessionExit(int iSessionID);
    //根据通话Id清空UI对应的数据指针session结束
    void ClearSessionData(int iSessionID);
    //根据通话Id获得通话时间数据
    int GetTalkTimeBySessionId(int nSessionId);
    //根据Logic层所提供的数据进行错误信息更新，若有错误信息
    void UpdateErrorInfo(void * pData);
    //显示弹框提示
    virtual void UpdateMsgboxNoteNotify(const yl::string strMsgboxNoteNotify);
    //获取输入内容
    bool GetInputContent(yl::string & strNum, yl::string & strDisplayname);

#if IF_TALKUI_USB_ENABLE
    //处理usb消息 连接状态改变
    //iState USB状态
    LRESULT OnUSBRecordMsgStateChange(int iState);
    //录制警告
    // waparm： 警告值（单位分钟）
    LRESULT OnUSBRecordMsgWarning(int iTime);
    //容量满 提醒
    LRESULT OnUSBRecordMsgNotenough();
    //录制截图 提醒
    //waparm：0=开始录制,1=停止录制,2=截图  lparam：成功与否(0/1)
    LRESULT OnRecordStorageRecordResult(int iType, int iTypeResult);
#endif

public:
    //创建通话UI
    CUILogicBaseTalkUI * CreateTalkUI(const yl::string & strName);
    //释放通话UI
    void ReleaseTalkUI(const yl::string & strProcessorName, CUILogicBaseTalkUI * pTalkUI);

    //释放ui缓存
    void ClearTalkUICache();

public:
    //刷新softkey
    void UpdateTalkUISoftkey();

#if IF_TALKUI_CONTACT_PHOTO
    //获取会议联系人头像
    yl::string GetPicContactsConference();

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //获取蓝牙联系人头像
    virtual yl::string GetPicBluetoothContact();
#endif

    virtual yl::string GetPicCloudContacts();
#endif


    //刷新title
    virtual void UpdateTalkUITitle(yl::string strTitle);

    //刷新pagenum显示
    //iCurrentIndex是实际的序号 strPageNum（UI直接显示该字符串）中的开始序号会在iCurrentIndex的基础上加1      bShowPageNum
    virtual void UpdateTalkUIPageNum();

    //刷新通话时间显示
    void UpdateTalkUITime(uint iHour, uint iMin, uint iSecond);

    //刷新通话图标
    //暂时是只有来电界面使用到
    void UpdateTalkUIIcons();

public:
    //设置talkuiframemanager实例指针
    void SetTalkUIFrameManager(CTalkUIFrameManagerBase * pTalkUIFrameManagerBase);

public:
    //获取当前的焦点通话Processor
    CBaseTalkUIProcessor * GetCurrentTalkUIProcessor();

public:
    //获取通道图标
    TALKUI_CHANNEL_ICON GetChannelIcon();
    //获取蓝牙图标
    //TALKUI_BLUETOOTH_ICON GetBlueToothIcon();

public:
    //刷新蓝牙连接状态
    void UpdateBlueToothIcon();
    //更新通道图标
    void UpdateChannelIcon();
    //刷新其他提示
    void UpdateOtherNotify(yl::string strOtherNotify);

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //更新ACD队列状态
    void UpdateACDQueueStatus();
#endif

    //检查网络，异常则给出提示，否则清除提示
    void CheckNetwork();
    //设置wifi信号不良提示，true则给出提示，false清除提示
    void SetWifiNotify(bool bIsReady);
    //检测是否显示安全等级
    void CheckSecurityClassification();

    void UpdateVolumebar(bool bShow);

protected:
    //清除所有的通话数据收到sessionID = -1的时候会调用
    void ClearTalkProcessor();
    //清除一个session
    void EraseTalkUIProcessor(int iSessionID);

private:
    // 根据逻辑层上传数据 创建UI
    CBaseTalkUIProcessor * CreateTalkUIByData(int iSessionID, const yl::string & strName);
    //收集 给TalkUI 的数据
    void CollectTalkUIData(DataLogic2UI & dataLogic2UI);

protected:
    //TalkUIFrameManager UI处理类
    CTalkUIFrameManagerBase * m_pTalkUIFrameManagerBase;

private:
    CBaseTalkUIProcessor * m_pCurrentProcessor; //当前显示的逻辑实例
    CUILogicToTalkUIData  m_dataUILogicToTalkUI;//UILogic给talkUI数据
    //注意: 只有在UpdateUI函数流程的时候可以使用其他地方禁止使用以免发生异常
    DataLogic2UI          m_objDataLogic2UI;    //临时保存逻辑层上传到UI 的数据
};

// 获取全局唯一的对象
#define _TalkUILogicFrameManager GET_SINGLETON_INSTANCE(CTalkUILogicFrameManager)

#endif // __TALKUI_FRAME_PROCESSOR_H__
