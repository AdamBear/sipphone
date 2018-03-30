#ifndef __BASE_TALK_UILOGIC_PROCESSOR_H__
#define __BASE_TALK_UILOGIC_PROCESSOR_H__

#include <ETLLib.hpp>
#include "commonunits/basedeletelater.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "keyguard/include/keyguard_common.h"
#include "talkuilogiccommon.h"

#include "voice/include/voicecommon.h"
//#include "uilogicbasetalkui.h"

class CUILogicBaseTalkUI;
class CBaseTalkUIProcessor : public CBaseDeleteLater, public CMKProcessor
{
public:
    //获取该逻辑类名称
    inline yl::string GetProcessorName()
    {
        return m_strProcessorName;
    }
    //获取SessionID
    inline int GetSessionID()
    {
        return m_iSessionID;
    }
    //设置SessionID
    virtual void SetSessionID(int iSessionID);

    virtual void SetSessionState(SESSION_STATE  eSessionState);
    //设置SessionID
    inline SESSION_STATE GetSessionState()
    {
        return m_eSessionState;
    }

public:
    explicit CBaseTalkUIProcessor(yl::string strName = "");
    virtual ~CBaseTalkUIProcessor();
    //构造完成后 绑定了UI后调用
    virtual void Init();
    //释放前解除UI绑定前调用
    virtual void Uninit();
    //设置绑定的UI
    virtual void SetBindUI(CUILogicBaseTalkUI * pTalkUI) = 0;
    //获取绑定的UI
    virtual CUILogicBaseTalkUI * GetBindUI() = 0;
    //设置成私有属性 不让子类调用
    //必须设置成私有类型 否则子类调用出错


public:
    ////设置数据之前做的一些公共的操作， 在这是先提取了联系人信息
    //这是把数据设置到processor
    void DealSetTalkData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

    //给子界面设置数据
    virtual void SetTalkUIData(UPDATE_REASON eReason = UPDATE_DEFAULT) = 0;

    virtual void OnLineSelectResult(LINE_SELECT_RESULT eResult, int nSelectIndex, int nSelectID) {}

    //当新的session和当前焦点session一致时，界面是否需要重刷
    virtual bool NeedReUpdate()
    {
        return true;
    }

private:
    // 各个子类去实现就可以
    //基本数据放在SetTalkData
    ////获取title信息 放在SetTitle
    //设置icon 放在SetIcons
    virtual void SetTalkData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT) = 0;

    //设置成私有属性不让子类调用
    //必须设置成私有类型 否则子类调用出错
private:
    //设置title
    virtual void SetTitle(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT) = 0;
    //设置图标列表
    virtual void SetIcons(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT) = 0;

    virtual void SetDNDIcon();

public:
    //获取输入内容
    virtual bool GetInputContent(yl::string & strNum, yl::string & strDisplayname)
    {
        return false;
    };
    //获取通话时间
    virtual int  GetTalkTime();
    // 获取record图标类型
    virtual ICON_TYPE GetRecordIconType()
    {
        return IT_UNKNOWN;
    }
    //从CallInfo 数据中获取 record图标类型
    ICON_TYPE GetRecordIconTypeFromCallInfo(CCallInfo * pCallInfo);
    //根据ICONType 获取 通话显示图标
    TALKUI_ICON GetTalkUIIconByIconType(ICON_TYPE eIconType);

    TALK_NETWORK_STATUS GetNetworkStatus()
    {
        return m_eNetworkStatus;
    }

public:
    // 从各个Routine获取需要显示的icon
    virtual void GetIconList(YLList<TALKUI_ICON> & listIcons)
    {
        listIcons = m_listIcons;
    };
    // 返回左上角标题
    virtual yl::string GetTitle()
    {
        return m_strTitle;
    };

    //返回自定义的通道图标
    virtual bool GetCustomChannelIcon(TALKUI_CHANNEL_ICON & eIconType)
    {
        return false;
    }

    // 返回通道图标状态
    TALKUI_CHANNEL_ICON GetChannelIcon();
    // 返回蓝牙图标状态
    //TALKUI_BLUETOOTH_ICON GetBluetoothIcon();

public:
    //刷新UI通道图标
    void UpdateUIChannelIcon();

    //刷新通话时间显示
    void UpdateTalkUITime();

    //刷新softkey
    void UpdateTalkSoftkey();

    //更新信息
protected:
    ////更新联系人信息
    //virtual void UpdateContactInfo(void* pData) = 0;


public:
    //获取softkey数据， GetSoftkeyData 内部调用GetSoftkey
    void GetTalkSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey);

protected:
    void PreLoadCustomSoftkey();
    virtual void RelayoutSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey) {}
    virtual void AdjustCustomSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);
    virtual bool AllowSoftkeyShow(SoftKey_TYPE eType);
    void CutEmptySoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

public:
    //softkey处理
    bool DealSoftkeyPress(SoftKey_TYPE eSoftkey);

    //安全等级操作结果处理
    virtual void DealSecurityResult(const yl::string & strSecurityLevel) {}

private:
    //这个函数设置成私有属性 不让子类调用
    //必须设置成私有类型 否则子类调用出错
    //softKey 按键处理
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftkey) = 0;

    //设置成私有属性不让子类调用
    //必须设置成私有类型 否则子类调用出错
private:
    //获取softkey数据信息
    virtual void GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey) = 0;

    //获取总是存在的softkey（不受softkey layout配置控制）
    //处于最前
    virtual void GetAlwaysBeforeSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey) {}
    //处于最后
    virtual void GetAlwaysAfterSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey) {}

protected:
#if IF_TALKUI_USB_ENABLE
    //添加 录制相关softkey
    bool AddUSBRecordSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    //添加 USB截图相关softkey
    void AddUSBScreenshotSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    //处理开始录音
    void DealBeginRecord();

    //处理停止录音
    void DealStopRecord();

    //处理暂停录音
    void DealPauseRecord();

    //处理恢复录音
    void DealResumeRecord();

    //处理截图
    void DealScreenshot();
#endif

    //获取当前话路的焦点联系人信息
    virtual bool GetFocusContactInfo(CContactInfo & objContactInfo);

protected:
    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey()
    {
        return true;
    }

    //设置成私有属性不让子类调用
    //必须设置成私有类型 否则子类调用出错
private:
    // softkey处理(自定义类型)
    virtual void GetSoftkey(yl::string & strFile, yl::string & strStatus) = 0;

protected:
    //获取softkey layout 的softkey数据
    bool LoadCustomSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey,
                           const yl::string & strFile,
                           const yl::string & strStatus);

    // 判断action是否需要播放按键音
    //硬按键也需要播放按键音
    //如果硬按键 和action 都判断要播放按键音的 那么 硬按键不播放了  统一由action播放
    //目前该函数 仅有 拨号界面使用
    virtual bool IsSoftkeyNeedToPlayKeyTone(SoftKey_TYPE softkeyType);

    //按键事件是否通过逻辑分开处理
    virtual bool AllowKeyPadResponseEvent(KEY_GUARD_TYPE eGuyardType, int nKeyCode, bool bPress);

    //softkey事件是否通过逻辑分开处理
    virtual bool AllowSoftkeyResponseEvent(SoftKey_TYPE eSoftkey, KEY_GUARD_TYPE eGuyardType);

    //按键处理
public:
    // 是否允许键盘事件通过
    virtual bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress);

    // 是否允许softkey事件通过
    bool AllowSoftKeyEventOnKeyLock(SoftKey_TYPE eSoftkey);

    // 判断是否需要播放按键音
    //action 也需要判断是否播放按键音
    //如果硬按键 和action 都判断要播放按键音的 那么 硬按键不播放了  统一由action播放
    virtual bool IsNeedToPlayKeyTone(int iKey);

public:
    // 通道键处理
    bool DealChannelKeyPress(int iKey);

    // 按键预处理
    virtual bool DealKeyPreProcess(int iKey);

    // 功能键处理
    bool DealFunckeyPress(int iKey);

    //处理数字键
    bool DealDigitkeyPress(int iKey);

    //按键弹起处理
    bool DealKeyRelease(int iKey)
    {
        return OnKeyRelease(iKey);
    }

    //触屏事件处理
    bool DealScreenPress();

    //UpdateWnd事件处理
    bool DealUpdateWnd();

    // 焦点改变.
    // | bFocused | [in] 为将要改变的焦点状态. true为获取焦点，false为失去焦点
    virtual void OnFocusChanged(bool bFocused);

    //这边就不用做了  直接在basetalkuiprocessor 的时候 也同时执行 volumnvalue 改变的事件
    ////音量条显示状态改变  要不然逻辑层不会刷新状态  ring界面的silence， softkey 按下 可能无效， 因为逻辑层认为当前已经处于silence状态了
    virtual void OnTalkVolumnVisibleChange(bool bVisible);

    //音量条变化调用函数       ！！！！！！！！！该部分 黑白屏还未实现 需要实现
    //发送音量变化的通知 给逻辑层
    //在来电界面 还需要显示 静音图标
    //音量条变化调用函数
    virtual void OnTalkVolumnValueChange();

    ////处理UI动作事件
    //virtual void OnUIAction(yl::string strAction) = 0;

    ////处理UI事件
    //void OnTalkUIAction(SoftKey_TYPE eSoftkeyType);

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    CCallInfo * GetFocusCallInfo();

    bool SetRecordSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    bool IsAllowDefineRecordSoftkey();

    virtual bool IfNeedCallRecording()
    {
        return false;
    }
#endif

protected:
    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);

    //必须设置成私有类型 否则子类调用出错
private:
    //这两个函数设置成 私有 属性  不让子类调用
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey) = 0;

    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey) = 0;

protected:
    //按键弹起处理
    virtual bool OnKeyRelease(int iKey)
    {
        return false;
    }

    //UpdateWnd处理
    virtual bool OnUpdateWnd()
    {
        return false;
    }

public:
    // 切换通话账号
    void ChangeAccount(int iAccountID, bool bRefresh);


public:
    // 是否需要锁住按键
    virtual bool IsNeedLockKey(int iLockType, int iKey);

    // 是否需要锁指定softkey
    virtual bool IsNeedLockSoftKey(int iLockType, int eSoftKey);

    // 是否*、#呼出键
    virtual bool IsSendDigitKey(yl::string strEditContent, int iKey);

    virtual void ShowDealLastNote();

    virtual bool IsShowSecurityClassification()
    {
        return false;
    }

public:
    //获取TargetInfo 信息 主要是给 通话、会议 提供新来电 targetinfo 信息  获取的优先级为 Label、DisplayName、UserName
    yl::string GetTargetInfo();

protected:
    // 通道按键是否退出
    virtual bool IsChannelKeyToExit(int iKey);

    // 是否允许给逻辑层发送Action  该函数配合 SendUIRequestAction 使用
    virtual bool IsAllowSendAction(TALK_ACTION_TYPE eActionType);


    //发送事件和数据给talklogic
    virtual bool SendUIRequestAction(TALK_ACTION_TYPE eActionType, WPARAM wActionParam = 0,
                                     LPARAM lActionParam = 0, void * pExtraData = NULL);



protected:
    //获取Label  Label 不存在那么 获取GetDisplayName 函数数据
    yl::string GetLabel(int iAccountID);

    //获取Displayname Display 如果获取不成功 那么用 GetUserName 函数数据
    yl::string GetDisplayName(int iAccountID);

    //获取UserName 数据
    yl::string GetUserName(int iAccountID);

protected:
    //添加通话图标  eTalkUIIcon表示需要添加的图标     bAdd=tru表示添加， bAdd=false表示删除
    void AddTalkUIIcon(TALKUI_ICON eTalkUIIcon, bool bAdd);

protected:
    // session status
    SESSION_STATE   m_eSessionState;
    // isession ID
    int             m_iSessionID;
    //通话子界面信息
    yl::string      m_strProcessorName;

protected:
    //要显示的title
    yl::string m_strTitle;
    // 需要显示的icon
    YLList<TALKUI_ICON> m_listIcons ;

protected:
    time_t  m_tStartTime;       //通话计时 开始时间
    bool m_bShowTalkTime;       //是否显示通话时间

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    CALL_RECORD_STATUE      m_eRecordStatue;
#endif

protected:
    TALK_NETWORK_STATUS m_eNetworkStatus;

protected:
    bool            m_bUseCustomSoftkey;
    YLVector<SoftKey_TYPE> m_vecCustomSoftkey;

public:
    //通道键 按下的时候是否要显示 音量条
    //当前的 通道模式
    static CHANNEL_MODE s_eChannelMode;
    //是否显示音量条
    static bool s_bIsShowVolume;

    static CHANNEL_MODE S_GetCurrentChannelMode();
    static void S_SetCurrentChannelMode(CHANNEL_MODE eChannelMode);

    static bool S_GetIsShowVolume();
    static void S_SetIsShowVolume(bool bIsShowVolume);
    //通道键 按下的时候是否要显示 音量条结束


};

#endif // __BASE_TALKUI_PROCESSOR_H__
