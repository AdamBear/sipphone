#ifndef _DLGRINGING_BASE_H_
#define _DLGRINGING_BASE_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgnormaltalkbase.h"

class CDlgRingingBase : public CDlgNormalTalkBase
{
    Q_OBJECT

public:
    //构造函数
    CDlgRingingBase();

    //析构函数
    virtual ~CDlgRingingBase();

public:
    //设置通话数据内容
    virtual void SetTalkData(DataLogic2UIBase * pData);

    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //是否显示音量条
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

    //是否显示DTMF条
    virtual bool IsShowDtmfLine() const;

    //是否显示加密图标,rectEncrypt hold the location of encrypt label
    virtual bool IsShowEncryptTip(void * pData, QRect & rectEncrypt);

    // 焦点切换
    virtual void OnFocusChanged(bool bFocus);

    bool IsShowTip();

    virtual void OnTipChange(TALK_TIP_TYPE eTipType);

    //处理UI事件
    virtual bool OnUIAction(const QString & strAction);

    // 事件过滤器
    bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey, bool bEXP = FALSE);
    //数字键处理
    virtual bool OnDigitkeyPress(int iKey);

    bool IsRecall()
    {
        return m_bIsRecalling;
    }

public:
    //音量条变化调用函数
    virtual void OnTalkVolumnValueChange();

protected:
    //设置Softkey数据Action值
    virtual void GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);
#if IF_FEATURE_CUSTOM_SOFTKEY
    virtual void GetDynamicSoftkey(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);
    virtual bool IsSoftkeyAllowToShow(const yl::string & strSoftkeyAction, DataLogic2UIBase * pData = NULL);
#endif

    // 初始化页面的数据
    void InitData();

    // 获取配置softkey的文件名和结点名
    void GetCustomSoftkeyData(yl::string & strFile, yl::string & strStatus);

    //获取蓝牙来电 softkey
    void GetBlueToothRingSoftkey(CArraySoftKeyBarData & objSoftkeyData, void * pData = NULL);

    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();

    void ProcessAnswerIncomingCall();

private:
    ////动画控件
    //qMultiPicAnimation*   m_pPicAnimation;

    int                 m_nCallID;

    bool                m_bIsSilence;

    // 是否recall状态
    bool                m_bIsRecalling;

    // 是否显示CallCenter Call Information
    bool                m_bCallCenterInfoAvaliable;

    // 记录call decline是否开启
    bool                m_bCallDeclineEnable;
    // 是否允许forward
    bool                m_bIsAllowForward;

#ifdef IF_FEATURE_EMERGENCY
    //记录 是否是紧急号码，并支持紧急回呼功能开启
    bool                m_bEmergencyCalled;
#endif

protected:
    bool m_bAnswered;       //是否已应答
};

#endif
