#pragma once

#include "basetalkproxy.h"
#include "yllist.h"
#include "talk/talklogic/include/uiandlogic_common.h"

class CCallInfo;
// 封装通话异步处理事件
class CallOutInfo
    : public CBaseTalkASyncProxy
{
public:

    typedef YLList<CallOutInfo*>    LIST_INFO;

    explicit CallOutInfo(CCallInfo* p, LIST_INFO& list);

    bool OnCallBack();

private:
    LIST_INFO&      m_list;

#if IF_BUG_27033 || IF_BUG_30965
    //改变to的头域
    void ChangeTo(yl::string& strFullName, DIAL_DSSKEY_TYPE eDialDsskeyType);
#endif

public:

    int             m_iAccountID;
    int             m_iCallID;
    TALK_CALLOUT_TYPE   m_eCallOutType;
    int             m_iMask;            // 呼出操作
    int             m_iBLAIndex;        // 大于0的值
    int             m_iBLFIndex;        // 不等于-1
    int             m_eBlaType;

    int             m_iEmergencyIndex;  // 紧急呼出对应的序号
    yl::string      m_strNumber;        // 呼出号码
    yl::string      m_strCode;          //ACD DispCode
    yl::string      m_strInvokeID;      // tiptel服务器控制话机呼出的id
    yl::string      m_strExtNumber;     //扩展号码
#if IF_FEATURE_CTRIP_CTI
    yl::string      m_strXUUI;
#endif
};

