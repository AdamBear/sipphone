#ifndef __ELEMENT_STATUS_H__
#define __ELEMENT_STATUS_H__

#ifdef IF_FEATURE_XMLBROWSER

#include <yllist.h>
#include "ylstring.h"

typedef YLList<XBStatusData *>   typeListStatus;

// typedef yl::map<yl::string, int>  Map_ID_Timeout;
// Element Status类
class CElementStatus : public CElementBase
{
public:
    CElementStatus();
    virtual ~CElementStatus();

    // 创建之后的处理
    virtual bool AfterCreate();
    // 销毁之前的处理
    virtual void BeforeDestroy();
    // 是否播放提示音
    virtual bool IsNeedPlayBeep();

private:
    // 实现数据转换
    IMPLEMENT_GETXDATA(XBStatusData);

    // 数据及获取数据接口设置为static public
public:
    // 接收并更新状态数据
    // | pData | [in] 如果接收成功，此数据由Status静态链表管理
    // 返回是否数据是否接收并更新成功
    static bool ReceiveStatusData(XBStatusData * pData);
    // 释放
    static void ReleaseStatusList();
    // 处理定时器
    static bool OnTimer(UINT uTimerID) DECLARE_ATTR;
    // 通知IdleScreen更新
    static void UpdateStatusView();

#if IF_FEATURE_START2START_ACD
    static void ReleaseStatus(const yl::string & strSessionId);
#endif

private:
    static typeListStatus    m_s_listStatus;
};

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif  // __ELEMENT_STATUS_H__
