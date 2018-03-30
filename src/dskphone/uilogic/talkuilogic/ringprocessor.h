#ifndef _RING_PROCESSOR_H
#define _RING_PROCESSOR_H

#include "basetalkuiprocessor.h"


struct tagRingUIData
{
    CContactInfo        m_objContactInfo;   //联系人信息

    yl::string
    m_strViaInfo;                //via信息
    bool m_bIsSilence;              //逻辑层上传的是否静音
    bool m_bIsRecalling;            //是否recall状态
    // 是否显示CallCenter Call Information
    bool m_bCallCenterInfoAvaliable;
    bool m_bIsVoiceKeyPress;        //是否在刷新数据后按了音量条键
    bool m_bAnswered;       //是否已应答
    // 记录call decline是否开启
    bool m_bCallDeclineEnable;
    bool m_bEmergencyCalled;

    bool m_bVideo;

    yl::string m_strTargetName;     //目标名称
    yl::string m_strTargetNumber;   //目标号码
    yl::string m_strTargetFullContactInfo;      //目标完整号码

    bool m_bAllowFwd;               //是否允许显示Fwd

    bool m_bAllowJoin;              //是否允许加入会议

    tagRingUIData()
    {
        Reset();
    }

    void Reset()
    {
        m_objContactInfo.Reset();

        m_strViaInfo = "";                  //via信息
        m_bIsSilence = false;               //逻辑层上传的是否静音
        m_bIsRecalling = false;         //是否recall状态
        // 是否显示CallCenter Call Information
        m_bCallCenterInfoAvaliable = false;
        m_bIsVoiceKeyPress = false;     //是否在刷新数据后按了音量条键
        m_bAnswered = false;        //是否已应答

        m_bCallDeclineEnable = false;
        m_bEmergencyCalled = false;

        m_strTargetName = "";       //目标名称
        m_strTargetNumber = ""; //目标号码
        m_strTargetFullContactInfo = "";        //目标完整号码
        m_bVideo = true;

        m_bAllowFwd = true;

        m_bAllowJoin = false;
    }

};

typedef struct tagRingUIData CRingUIData;

class CUILogicRingUIBase;
class CRingProcessor : public CBaseTalkUIProcessor
{
public:
    CRingProcessor();
    virtual ~CRingProcessor();

public:
    //构造完成后 绑定了UI后调用
    virtual void Init();
    //释放前解除UI绑定前调用
    virtual void Uninit();
    //设置绑定的UI
    virtual void SetBindUI(CUILogicBaseTalkUI * pTalkUI);
    //获取绑定的UI
    virtual CUILogicBaseTalkUI * GetBindUI();


    //获取保存的talkingui数据
    CRingUIData * GetRingUIData();

#if IF_FEATURE_NFON_RTP
    //发送事件和数据给talklogic
    virtual bool SendUIRequestAction(TALK_ACTION_TYPE eActionType, WPARAM wActionParam = 0,
                                     LPARAM lActionParam = 0, void * pExtraData = NULL);
#endif

public:
    //获取转接信息
    yl::string GetVIAInfo();

private:
    //设置数据
    virtual void SetTalkData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

public:
    //这边就不用做了  直接在basetalkuiprocessor 的时候 也同时执行 volumnvalue 改变的事件
    ////音量条显示状态改变  要不然逻辑层不会刷新状态  ring界面的silence， softkey 按下 可能无效， 因为逻辑层认为当前已经处于silence状态了
    //virtual void OnTalkVolumnVisibleChange(bool bVisible);

    //音量条变化调用函数
    virtual void OnTalkVolumnValueChange();

protected:
    //处理音量改变
    void DealVolumnChange();

protected:
    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);

private:
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey);

    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey);

    //softKey 按键处理
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftkey);

    //设置title
    virtual void SetTitle(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //设置图标列表
    virtual void SetIcons(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //给子界面设置数据
    virtual void SetTalkUIData(UPDATE_REASON eReason = UPDATE_DEFAULT);

    //获取softkey数据信息
    virtual void GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    //获取总是存在的softkey（不受softkey layout配置控制）
    //处于最前
    virtual void GetAlwaysBeforeSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);
    //处于最后
    virtual void GetAlwaysAfterSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    virtual void RelayoutSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey);

    //获取蓝牙来电 softkey
    void GetBlueToothRingSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();
    virtual bool AllowSoftkeyShow(SoftKey_TYPE eType);

    bool IsAllowSilenceSoftkey();
    bool IsAllowCallFwdSoftkey();
    bool IsAllowACDInfoSoftkey();

    // softkey处理(自定义类型)
    virtual void GetSoftkey(yl::string & strFile, yl::string & strStatus);

protected:
    // 应答来电
    void AnswerIncomingCall(bool bChannelKey = false);

protected:
    //保存的UI指针
    CUILogicRingUIBase * m_pTalkUI;
    //保存的UI数据
    CRingUIData m_objRingUIData;

    //ACD Softkey是否已经添加过
    bool                    m_bACDSoftkeySet;

};


#endif
