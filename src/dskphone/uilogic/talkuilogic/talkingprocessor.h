#ifndef _TALKING_PROCESSOR_H
#define _TALKING_PROCESSOR_H

#include "basetalkuiprocessor.h"

struct tagTalkingUIData
{
    CContactInfo        m_objContactInfo;   //联系人信息
    bool                m_bIsVideoTalk;     //是否是视频通话
    bool                m_bTransferFailed;  // 是否处于Transfer失败状态
    bool                m_bLocalHold;       // 是否处于Hold对方状态
    bool                m_bRemoteHold;      // 是否处于对方Hold状态
    bool                m_bVideoMute;           //是否视频静音
    bool                m_bAudioMute;       //是否音频静音
    bool                m_bHasCallComming; //有新来电需要提醒
    yl::string          m_strIncomingCallNum;   //新来电号码
    bool                m_bCallParkFailed;   // Park通话是否失败
    yl::string          m_strParkFailedInfo; // Park失败的原因
    yl::string
    m_strParkFailedInfoFirst;   //park失败原因的第一部分       //因为t46 在有头像时失败原因要分两行显示  这边为兼容t46
    yl::string          m_strParkFailedInfoSecond;  //park失败原因的第二部分
    TALK_STATE          m_eTalkState;      // 通话状态
    int                 m_nCallID;         //通话CallID
//  ICON_TYPE           m_eRecrodIcon;  // 录音图标类型

    bool                m_bAllowTran;   //是否允许trans
    bool                m_bAllowSwap;   //是否允许 swap
    bool                m_bAllowConf;   //是否允许组建会议
    int                 m_iSipAccount;  //sip账号序号
    BLA_TYPE            m_blaType;      //BLA类型
    CALL_TYPE           m_callType;     //通话类型

    bool                m_bEncrypt;     //是否通话加密

    bool                m_bSecurity;    //是否显示安全等级

//  bool                m_bVoiceEncryt;     //通话语音是否加密
//  bool                m_bIsHD;            //是否高清通话
    AUDIO_CODEC_TYPE m_eAudioCodec;//高清类型

    // 通话对应的UI类型
    TALKING_UI_TYPE     m_eTalkUIType;

    //是否对方正在请求视频通话
    bool        m_bRespondVideo;
    //是否自身正在请求视频通话
    bool        m_bRequestVideo;
    //是否允许切到视频通话
    bool        m_bCanSwitchVideo;
    //是否允许开视频
    bool        m_bCanStartVideo;
    //是否允许关视频
    bool        m_bCanStopVideo;
    //请求失败
    bool        m_bRequestFailed;

    bool                m_bAllowHold;       //是否允许hold  包括是否允许resume
    bool
    m_bInTransferCanHold;   //在被transfer的过程中根据标志判断是否可以hold
    bool                m_bAllowNewCall;    //是否允许newcall

    bool                m_bAllowJoin;   //是否允许加入到本地会议

#if IF_TALKUI_PSTN
    bool                m_bPstnFlashing;  // PSTN通话是否处于Flash状态
#endif

    tagTalkingUIData()
    {
        Reset();
    }

    void Reset()
    {
        m_objContactInfo.Reset();   //联系人信息重置
        m_bIsVideoTalk = false;     //是否是视频通话
        m_bTransferFailed = false;  // 是否处于Transfer失败状态
        m_bLocalHold = false;       // 是否处于Hold对方状态
        m_bRemoteHold = false;      // 是否处于对方Hold状态
        m_bVideoMute = false;           //是否视频静音
        m_bAudioMute = false;       //是否音频静音
        m_bHasCallComming = false; //有新来电需要提醒
        m_strIncomingCallNum = "";  //新来电号码
        m_bCallParkFailed = false;   // Park通话是否失败
        m_strParkFailedInfo = ""; // Park失败的原因
        m_strParkFailedInfoFirst =
            "";  //park失败原因的第一部分       //因为t46 在有头像时失败原因要分两行显示  这边为兼容t46
        m_strParkFailedInfoSecond = ""; //park失败原因的第二部分
        m_eTalkState = TS_UNKNOW;      // 通话状态
        m_nCallID = -1;        //通话CallID
//      m_eRecrodIcon = IT_UNKNOWN; // 录音图标类型
        m_bAllowTran = false;   //是否允许trans
        m_bAllowSwap = false;   //是否允许 swap
        m_bAllowConf = false;   //是否允许 组建会议
        m_iSipAccount = -1;
        m_blaType = BLA_NONE;       //BLA类型
        m_callType = CT_UNKNOW;     //通话类型

//      m_bVoiceEncryt = false;     //先设置成不加密
//      m_bIsHD = false;            //非高清通话
        m_eAudioCodec = ACT_NONE;

        // 通话对应的UI类型
        m_eTalkUIType = TALKING_UI_UNKNOWN;
        m_bAllowHold = false;       //是否允许hold

        m_bInTransferCanHold = false;

        m_bEncrypt = false;
        m_bSecurity = false;
        m_bAllowNewCall = false;    //是否允许newcall

        m_bAllowJoin = false;

#if IF_TALKUI_PSTN
        m_bPstnFlashing = false;
#endif

        m_bRespondVideo = false;
        m_bRequestVideo = false;
        m_bCanSwitchVideo = false;
        m_bCanStartVideo = false;
        m_bCanStopVideo = false;
        m_bRequestFailed = false;
    }

};

typedef struct tagTalkingUIData CTalkingUIData;

class CUILogicTalkingUIBase;
class CTalkingProcessor : public CBaseTalkUIProcessor
{
public:
    CTalkingProcessor();
    virtual ~CTalkingProcessor();

public:
    //构造完成后 绑定了UI后调用
    virtual void Init();
    //释放前解除UI绑定前调用
    virtual void Uninit();
    //设置绑定的UI
    virtual void SetBindUI(CUILogicBaseTalkUI * pTalkUI);
    //获取绑定的UI
    virtual CUILogicBaseTalkUI * GetBindUI();
    //获取callid
    int GetCallID();
    //是否是视频通话
    bool IsVideoTalk();
    //获取是否是transferfiled
    bool IsTransferFailed();
    //获取是否parkfailed
    bool IsParkFailed();
    //是否hold
    bool IsHold();
    //是否转移后的通话状态
    bool IsTransferred();

    //获取保存的talkingui数据
    CTalkingUIData * GetTalkingUIData();

    virtual void DealSecurityResult(const yl::string & strSecurityLevel);

public:
    //resume通话
    void ResumeTalk();

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    virtual bool IfNeedCallRecording();
#endif

protected:
    //获取当前话路的焦点联系人信息
    virtual bool GetFocusContactInfo(CContactInfo & objContactInfo);

protected:
    //设置数据
    virtual void SetTalkData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

protected:
    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);

public:
    // 按键预处理
    virtual bool DealKeyPreProcess(int iKey = 0);

    //返回自定义的通道图标
    virtual bool GetCustomChannelIcon(TALKUI_CHANNEL_ICON & eIconType);

private:
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey);

    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey);

protected:
    //softKey 按键处理
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftkey);

    ////处理UI动作事件
    //virtual void OnUIAction(yl::string strAction);

protected:
    //设置title
    virtual void SetTitle(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //设置图标列表
    virtual void SetIcons(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //给子界面设置数据
    virtual void SetTalkUIData(UPDATE_REASON eReason = UPDATE_DEFAULT);

private:
    //获取softkey数据信息
    virtual void GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

protected:
    //获取总是存在的softkey（不受softkey layout配置控制）
    //处于最前
    virtual void GetAlwaysBeforeSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);
    //处于最后
    virtual void GetAlwaysAfterSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    virtual void RelayoutSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey);

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //获取蓝牙softkey数据信息
    void GetBluetoothSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);
#endif

    // softkey处理(自定义类型)
    virtual void GetSoftkey(yl::string & strFile, yl::string & strStatus);

    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();
    virtual bool AllowSoftkeyShow(SoftKey_TYPE eType);

    bool IsAllowSwapSoftkey();
    bool IsAllowConfSoftkey();
    bool IsAllowParkSoftkey();
    bool IsAllowGroupParkSoftkey();
    bool IsAllowRTPSoftkey();

    //是否显示resume的softkey
    //因为transered 的判断较复杂  所以特别使用一个函数来判断
    bool IsShowResumeSoftkey();

    //是否显示Hold的softeky
    bool IsShowHoldSoftkey();

    virtual bool IsShowSecurityClassification();

protected:
    //获取通话状态的softkey
    virtual void GetTalkStatusSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    //获取来电状态的softkey
    void GetRingStatusSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //获取通话状态的softkey
    void GetBluetoothTalkStatusSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);
#endif

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //获取来电状态的softkey
    void GetBluetoothRingStatusSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);
#endif

    //设置ACD相关的softkey
    void SetACDSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey, int iAccountID);

    // 添加Broadsoft Park功能的Softkey
    void SetParkSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

protected:
    //根据通话UI类型更新标题提示
    yl::string GetTitleByTalkingUIType(TALKING_UI_TYPE eTalkUIType);

protected:
    //保存的UI指针
    CUILogicTalkingUIBase * m_pTalkUI;
    //保存的UI数据
    CTalkingUIData m_objTalkingUIData;

    bool                    m_bRTPSoftkeySet;
};

#endif
