//////////////////////////////////////////////////////////////////////////
//  callforwardnotreachableprocess.h
//  CFNR: CallForwardNotReachable
//  By yuyt! 2014/8/12

#ifndef __CFNR__PROCESSOR__H__
#define __CFNR__PROCESSOR__H__

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE

#include <ylhashmap.h>
#include "xmlparser/xmlparser.hpp"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

class CCFNRProcessor : public XsiBaseProcesser
{
public:
    static CCFNRProcessor * GetInstance();

    //设置Forward Not Reachable
    bool SetCFNR(bool bState, const char * szForwardNumber, int nLineID);

    //获取服务器上的Forward Not Reachable信息
    bool QueryCFNR(int nLineID);

    //获取上次从服务器请求的Forward Not Reachable信息
    void GetCFNRInfo(SCallForwardNotReachable & cfnrInfo, int nLineID);

private:
    CCFNRProcessor(void);
    virtual ~CCFNRProcessor(void);

    //解析XML数据
    bool ParseReponseCmdFromFile(const yl::string & strFilePath);

    //保存要提交到服务器的数据到文件,如果命令类型不需要发送内容时返回true
    bool SaveRequestCmdToFile(const yl::string & strFilePath);

    //override.
    virtual bool ProcessByType(XsiAction * pAction);

    //保存服务器返回的结果
    void SaveCFNR(const SCallForwardNotReachable & sCFNR, int nLineID);

private:
    SCallForwardNotReachable m_sCFNR;

    //根据账号保存其Call Forward Not Reachable信息
    YLHashMap<int, SCallForwardNotReachable> m_mapCFNR;
};

#define g_pCFNRProcessor (CCFNRProcessor::GetInstance())

#endif // IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
#endif // __CFNR__PROCESSOR__H__ 
