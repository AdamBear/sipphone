#ifndef _DLG_PARK_NOTIFY_BASE_H_
#define _DLG_PARK_NOTIFY_BASE_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgnormaltalkbase.h"


class CDlgParkNotifyBase : public CDlgNormalTalkBase
{
public:
    CDlgParkNotifyBase();

    virtual ~CDlgParkNotifyBase();

    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    // 初始化数据
    void InitData();

    //设置通话数据内容
    virtual void SetTalkData(DataLogic2UIBase * pData);

    //获取通话时间内容
    virtual QCustomTime GetTalkTime();

    //处理UI事件
    virtual bool OnUIAction(const QString & strAction);

    // 事件过滤
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

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
        return false;
    }

    virtual void GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);

};

#endif // _DLG_PARK_NOTIFY_BASE_H_
