//////////////////////////////////////////////////////////////////////////
//  simultaneousringprocess.h
//  SRP: Simultaneous Ring Personal
//  By yuyt, 2014/8/18

#ifndef __SIMULTANEOUS_RING_PROCESSOR__HEADER__
#define __SIMULTANEOUS_RING_PROCESSOR__HEADER__

#include <ylhashmap.h>
#include "xmlparser/xmlparser.hpp"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

class CSimultaneousRingProcessor
    : public XsiBaseProcesser
{
public:
    static CSimultaneousRingProcessor & GetInstance();

    //设置Simultaneous Ring Personal
    bool SetSRP(const SSimultaneousRing & simRingInfo);

    //获取服务器上的Simultaneous Ring Personal信息，返回的结果是通过发送
    bool QuerySRP(int nLineID);

    //获取上次从服务器得到的Simultaneous Ring Personal信息
    bool GetSRPInfo(SSimultaneousRing & simRingInfo);

    //检查号码在Simultaneous Ring Personal里是否已经存在
    bool CheckNumberExit(const char * szNumber);

private:
    CSimultaneousRingProcessor(void);
    ~CSimultaneousRingProcessor(void);

    //override
    virtual bool ProcessByType(XsiAction * pAction);

    //解析
    bool ParseReponseCmdFromFile(const yl::string & strFilePath);

    //解析Location list
    bool ParseSimRingLocation(const xml_node & nodeLocations);

    //保存cmd到文件
    bool SaveRequestCmdToFile(const yl::string & strFilePath);

    //保存服务器返回的结果
    void SaveSRPInfo(const SSimultaneousRing & simRingInfo, int nLineID);

private:
    //根据账号保存其Simultaneous Ring信息
    YLHashMap<int, SSimultaneousRing> m_mapSimRing;

    //Simultaneous Ring Personal Info
    SSimultaneousRing m_simRingInfo;
};

#define g_refSRPProcessor (CSimultaneousRingProcessor::GetInstance())

#endif

