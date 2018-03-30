#ifndef _DLG_CONFERENCE_BASE_H_
#define _DLG_CONFERENCE_BASE_H_

//mod-Talk
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgnormaltalkbase.h"
#include "commonwidget.h"

//前向声明


class CDlgConferenceBase : public CDlgNormalTalkBase
{
    Q_OBJECT

public:
    //构造函数，需传递UI名与UI类型
    CDlgConferenceBase();

    //析构函数
    virtual ~CDlgConferenceBase();

public:
    //设置通话数据内容
    virtual void SetTalkData(DataLogic2UIBase * pData);

    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

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
    virtual bool OnUIAction(const QString & strAction);

    // 事件过滤器
    bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 获取record图标类型
    virtual ICON_TYPE GetRecordIconType()
    {
        return m_eRecrodIcon;
    }

    // 是否有显示提示信息
    virtual bool IsShowTip()
    {
        return m_bHasCallComming;
    }


    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey, bool bEXP = FALSE);
    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey);

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    CCallInfo * GetFocusCallInfo();

    bool SetRecordSoftkey(CArraySoftKeyBarData & objSoftkeyData);

    bool IsAllowDefineSoftkey();
#endif

protected:
    // 初始化页面数据
    void InitData();

    //设置Softkey数据Action值
    virtual void GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);
#if IF_FEATURE_CUSTOM_SOFTKEY
    virtual bool IsSoftkeyAllowToShow(const yl::string& strSoftkeyAction, DataLogic2UIBase * pData = NULL);
    virtual void GetDynamicSoftkey(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);
#endif

    virtual void RefreshRecordSoftKey(CArraySoftKeyBarData & objSoftkeyData);

    //设置Softkey数据Action值
    //talk状态下的softkey data
    bool GetTalkStatusSoftKeyBarData(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData);

    //ring状态下的softkey data
    bool GetRingStatusSoftKeyBarData(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData);

    // 获取配置softkey的文件名和结点名
    void GetCustomSoftkeyData(yl::string & strFile, yl::string & strStatus);

    // 添加Broadsoft ACD功能的Softkey
    void SetACDSoftkey(CArraySoftKeyBarData & objSoftkeyData, YLList<CCallInfo *> & listCallInfo);

    //获取蓝牙softkey数据信息
    void GetBlueToothConnectSoftkey(CArraySoftKeyBarData & objSoftkeyData, void * pData = NULL);

    //conference 特有的接口
protected:
    virtual int GetCurrentActionCallID() = 0;

protected:
    bool                  m_bLocalHold;     // 是否处于Hold对方状态
    ICON_TYPE             m_eRecrodIcon;    // 录音图标类型
    bool                  m_bHasCallComming; //有新来电需要提醒
    int                   m_nFirstCallId;   //第一路通话的id

    bool                  m_bVoiceRecord;  // 是否开启voice record
    bool                m_bSecurity;        //是否显示安全等级

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    CALL_RECORD_STATUE      m_eRecordStatue;
#endif

    bool                m_bCallingRoutineExist; //是否存在calling话路
    int                 m_nLastCallingCallId;   //最后呼出中的callid
};

#endif // _DLG_CONFERENCE_UI_H_
