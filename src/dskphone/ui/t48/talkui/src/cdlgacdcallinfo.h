/************************************************************************
 * FileName  : CDlgACDCallInfo.h (header file)
 * Purpose   :
 * Date      : 2012/12/24 9:50:39
 ************************************************************************/

#ifndef _CDLGACDCALLINFO_H_
#define _CDLGACDCALLINFO_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "cdlgacdcallinfobase.h"

class CDlgACDCallInfo : public CDlgACDCallInfoBase
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgACDCallInfo();
    }

public:
    CDlgACDCallInfo();

    virtual ~CDlgACDCallInfo();

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //设置通话数据内容
    virtual void SetTalkData(DataLogic2UIBase * pData);

    // 是否有显示提示信息
    virtual bool IsShowTip();

    // NetWork、Dtmf、音量条等Tip显示状态变化时的处理
    virtual void OnTipChange(TALK_TIP_TYPE eTipType);

protected:
    // 初始化页面数据
    void InitData();

protected:
    bool
    m_bIsShowLongestLab;        //是否显示有 m_labCallsLongestLab; 控件  主要是在显示策略判断的时候使用

private:
    QLabel          m_labWaitTimeLab;
    QLabel          m_labCCNameLab;
    QLabel          m_labCCUIDLab;
    QLabel          m_labQueueLab;
    QLabel          m_labCallsLongestLab;

};

#endif
