#ifndef _DLG_PARK_NOTIFY_UI_H_
#define _DLG_PARK_NOTIFY_UI_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgparknotifybase.h"
#include "commonwidget.h"

//mod-uicommon
#include "uicommon/uikernel/qtimepiece.h"



class CDlgParkNotifyUI: public CDlgParkNotifyBase
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgParkNotifyUI();
    }

public:
    CDlgParkNotifyUI();
    virtual ~CDlgParkNotifyUI(void);

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //给界面设置数据
    virtual void SetTalkData(DataLogic2UIBase * pData);

    //// NetWork、Dtmf、音量条等Tip显示状态变化时的处理
    //virtual void OnTipChange(TALK_TIP_TYPE eTipType);

public:
    //parknotify界面使用
    //获取通话时间内容
    virtual QCustomTime GetTalkTime();

protected:
    //构造函数调用初始化数据
    void InitData();

protected:
    QTimePiece          m_qTimePiece;    // 计时控件
    IconLabel           m_qIconLabel;   // 显示Park提示信息的Icon
    QLabel              m_qCoverLabel;  // 使联系人头像变成灰色的控件

    // 无头像时使用
    QLabel              m_qParkIcon;    //Parked提示图标
    QLabel              m_qParkTip;     //Parked提示信息
};

#endif // _DLG_PARK_NOTIFY_UI_H_
