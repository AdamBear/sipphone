//////////////////////////////////////////////////////////////////////////
// mobilityprocessor.h
// MBLT: MoBiLiTy
// By yuyt, 2014/8/15

#ifndef __MOBILITY__PROCESSOR__HEADER__
#define __MOBILITY__PROCESSOR__HEADER__

#include <ylhashmap.h>
#include "xmlparser/xmlparser.hpp"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

class CMobilityProcessor
    : public XsiBaseProcesser
{
public:
    static CMobilityProcessor & GetInstance();

    //设置mobility
    bool SetMobility(const SMobility & mobilityInfo, int nLineID);

    //获取服务器上的Moblity信息，返回的结果是通过发送MBLT_MESSAGE_GET_RESULT消息来传送的
    bool  QueryMobility(int nLineID);

    //获取上次从服务器请求的Mobility信息
    void  GetMobilityInfo(SMobility & mobilityInfo, int nLineID);
private:
    CMobilityProcessor(void);
    ~CMobilityProcessor(void);

    //override, 处理执行cmd之后从服务器返回的结果
    virtual bool ProcessByType(XsiAction * pAction);

    //解析XML数据
    virtual bool ParseReponseCmdFromFile(const yl::string & strFilePath);

    //保存要提交到服务器的数据到文件,如果命令类型不需要发送内容时返回true
    virtual bool SaveRequestCmdToFile(const yl::string & strFilePath);

    //保存服务器返回的结果
    void SaveMobilityInfo(const SMobility & mobilityInfo, int nLineID);

private:
    //根据账号保存其Mobility信息
    YLHashMap<int, SMobility> m_mapMobilityInfo;

    SMobility m_mobilityInfo;
};

#define g_refMBLTProcessor (CMobilityProcessor::GetInstance())

#endif

