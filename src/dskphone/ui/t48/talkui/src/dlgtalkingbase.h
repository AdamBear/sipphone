#ifndef _DLG_TALKING_BASE_H_
#define _DLG_TALKING_BASE_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgnormaltalkbase.h"
#include "commonwidget.h"
#include "uicommon/messagebox/messageboxbase.h"

class CDlgTalkingBase : public CDlgNormalTalkBase
{
    Q_OBJECT

public:
    //构造函数，需传递UI名与UI类型
    CDlgTalkingBase();
    //析构函数
    virtual ~CDlgTalkingBase();

public:
    //设置通话数据内容
    virtual void SetTalkData(DataLogic2UIBase * pData);

    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //是否显示音量条
    bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                         QRect & rtVolume, yl::string & strBGPath) const;

    //获取通话时间
    virtual QCustomTime GetTalkTime();

    //是否显示DTMF条
    virtual bool IsShowDtmfLine() const;

    //是否显示加密图标,rectEncrypt hold the location of encrypt label
    virtual bool IsShowEncryptTip(void * pData, QRect & rectEncrypt);

    // NetWork、Dtmf、音量条等Tip显示状态变化时的处理
    virtual void OnTipChange(TALK_TIP_TYPE eTipType);

    // 焦点切换
    virtual void OnFocusChanged(bool bFocus);

    //处理UI事件
    virtual bool OnUIAction(const QString & strUIAction);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 获取record图标类型
    virtual ICON_TYPE GetRecordIconType()
    {
        return m_eRecrodIcon;
    }

    // 是否有显示提示信息
    virtual bool IsShowTip();

    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey, bool bEXP = FALSE);
    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey);

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    bool SetRecordSoftkey(CArraySoftKeyBarData & objSoftkeyData);
#endif

protected:
    // 初始化页面数据
    void InitData();

    //设置Softkey数据Action值
    //talk状态下的softkey data
    bool GetTalkStatusSoftKeyBarData(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData);

    //ring状态下的softkey data
    bool GetRingStatusSoftKeyBarData(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData);

    void GetBluetoothSoftkey(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);

    //获取通话状态的softkey
    void GetBluetoothTalkStatusSoftkey(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData);

    //获取来电状态的softkey
    void GetBluetoothRingStatusSoftkey(CArraySoftKeyBarData & objSoftkeyData, void * pData);

    virtual void GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);
#if IF_FEATURE_CUSTOM_SOFTKEY
    virtual void GetDynamicSoftkey(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);
    virtual bool IsSoftkeyAllowToShow(const yl::string& strSoftkeyAction, DataLogic2UIBase * pData = NULL);
#endif

    virtual void RefreshRecordSoftKey(CArraySoftKeyBarData & objSoftkeyData);

    //根据通话UI类型更新标题提示
    void UpdateTip(TALKING_UI_TYPE eTalkUIType);

    // 是否运行自定义softkey
    bool IsAllowDefineSoftkey();

    // 获取配置softkey的文件名和结点名
    void GetCustomSoftkeyData(yl::string & strFile, yl::string & strStatus);

    // 添加Broadsoft ACD功能的Softkey
    void SetACDSoftkey(CArraySoftKeyBarData & objSoftkeyData, int iAccountID);

    // 添加Broadsoft Park功能的Softkey
    void SetParkSoftkey(CArraySoftKeyBarData & objSoftkeyData);

    // 添加RTP Softkey
    bool SetRTPSoftkey(CArraySoftKeyBarData & objSoftkeyData, const DataTalking2UI * pTalkData);

protected:
    bool                m_bTransferFailed;  // 是否处于Transfer失败状态
    bool                m_bLocalHold;       // 是否处于Hold对方状态
    bool                m_bRemoteHold;      // 是否处于对方Hold状态
    bool                m_bHasCallComming; //有新来电需要提醒
    bool                m_bCallParkFailed;   // Park通话是否失败
    bool                m_bCallPushFailed;  // 是否显示CallPush失败提示信息
    yl::string          m_strParkFailedInfo; // Park失败的原因
    TALK_STATE          m_eTalkState;      // 通话状态
    int                 m_nCallID;         //通话CallID
    ICON_TYPE           m_eRecrodIcon;  // 录音图标类型

    // 通话对应的UI类型
    TALKING_UI_TYPE     m_eTalkUIType;
    bool                m_bSecurity;        //是否显示安全等级
    CALL_TYPE       m_eCallType;        // 类型（呼入、呼出）
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    CALL_RECORD_STATUE      m_eRecordStatue;
#endif
};
#endif // _DLG_TALKING_BASE_
