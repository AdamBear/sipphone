//////////////////////////////////////////////////////////////////////////
//  groupnightforwardprocess.h
//  GNF: GroupNightForward
//  By yuyt! 2014/8/12

#ifndef __GROUP_NIGHT_FORWARD_PROCESSOR__H__
#define __GROUP_NIGHT_FORWARD_PROCESSOR__H__

#include <ylhashmap.h>
#include "xmlparser/xmlparser.hpp"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

class CGNFProcessor : public XsiBaseProcesser
{
public:
    static CGNFProcessor * GetInstance();

    //获取服务器上的Group Night Forward信息
    bool  QueryGNF(int nLineID);

    //获取上次从服务器请求的Group Night Forward信息
    void  GetGNFInfo(SGroupNightForward & gnfInfo, int nLineID);
private:
    CGNFProcessor(void);
    ~CGNFProcessor(void);

    //解析XML数据
    bool ParseReponseCmdFromFile(const yl::string & strFilePath);

    //保存服务器返回的结果
    void SaveGNFInfo(const SGroupNightForward & gnfInfo, int nLineID);

    //override
    virtual bool ProcessByType(XsiAction * pAction);
private:
    //根据账号保存其GroupNightForward信息
    YLHashMap<int, SGroupNightForward> m_mapGnfInfo;

    SGroupNightForward m_gnfInfo;
};

#define g_pGNFProcessor (CGNFProcessor::GetInstance())

#endif
