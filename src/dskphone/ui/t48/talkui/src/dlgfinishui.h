#ifndef _DLG_FINISHUI_H_
#define _DLG_FINISHUI_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgfinishbase.h"
#include "commonwidget.h"

//前向声明
class CContactInfoFrame;
class IconLabel;
class QTimer;

class CDlgFinishedUI: public CDlgFinishedBase
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgFinishedUI();
    }

public:
    CDlgFinishedUI();
    virtual ~CDlgFinishedUI(void);

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //给界面设置数据
    virtual void SetTalkData(DataLogic2UIBase * pData);

    // 是否有显示提示信息
    virtual bool IsShowTip();

    // NetWork、Dtmf、音量条等Tip显示状态变化时的处理
    virtual void OnTipChange(TALK_TIP_TYPE eTipType);

    //设置区域
    virtual void AdjustLayout();

public:
    //Finish界面使用
    // 更新自动重拨提示语
    virtual void UpdateAutoRedialNote();

protected:
    //构造函数调用初始化数据
    void InitData();



protected:
    //通话时间提示
    QLabel      m_qLabelTalkTime;

    //显示失败图标和提示语
    CItemLabel m_qLabelInfo;

};

#endif // _DLG_FINISHUI_H_
