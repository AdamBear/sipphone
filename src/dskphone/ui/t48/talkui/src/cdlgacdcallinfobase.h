/************************************************************************
 * FileName  : CDlgACDCallInfo.h (header file)
 * Purpose   :
 * Date      : 2012/12/24 9:50:39
 ************************************************************************/

#ifndef _CDLGACDCALLINFO_BASE_H_
#define _CDLGACDCALLINFO_BASE_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgnormaltalkbase.h"

class CDlgACDCallInfoBase : public CDlgNormalTalkBase
{
    Q_OBJECT

public:
    CDlgACDCallInfoBase();

    virtual ~CDlgACDCallInfoBase();

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //设置通话数据内容
    virtual void SetTalkData(DataLogic2UIBase * pData);

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

protected:
    // 是否运行自定义softkey
    virtual bool IsAllowDefineSoftkey()
    {
        return false ;
    }
    //设置Softkey数据Action值
    virtual void GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);

    // 初始化页面数据
    void InitData();


protected slots:
    // 定时器处理函数
    void OnTimer();

protected:
    bool m_bAnswered;       //是否已应答

private:
    //自动退出定时器
    QTimer     *    m_pQuitTimer;
};

#endif
