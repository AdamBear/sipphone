// callwaitingprocess.h
// By yuyt, 2014/9/16.
#ifndef __CALL_WAITTING_PROCESS_HEADER__
#define __CALL_WAITTING_PROCESS_HEADER__

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING

#include <ylhashmap.h>
#include "xmlparser/xmlparser.hpp"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

class CCallWatingProcessor : public XsiBaseProcesser
{
public:
    static CCallWatingProcessor & GetInstance();

    //设置CallWating
    bool SetCallWatingState(bool bActive, int nLineID);

    //获取服务器上的CallWating信息
    bool QueryCallWating(int nLineID);

    //获取上次从服务器请求的CallWating信息
    bool GetCallWatingState(int nLineID);

private:
    CCallWatingProcessor(void);
    ~CCallWatingProcessor(void);

    //解析XML数据
    bool ParseReponseCmdFromFile(const yl::string & strFilePath);

    //保存要提交到服务器的数据到文件
    bool SaveRequestCmdToFile(const yl::string & strFilePath);

    //override.
    virtual bool ProcessByType(XsiAction * pAction);
private:
    YLHashMap<int, bool> m_mapCallWaitingInfo;
    bool m_bActive;
};

#define g_refCallWatingProcessor (CCallWatingProcessor::GetInstance())

#endif //IF_FEATURE_BROADSOFT_CALL_WAITING
#endif //__CALL_WAITTING_PROCESS_HEADER__
