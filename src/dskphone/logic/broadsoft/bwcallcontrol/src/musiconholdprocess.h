//////////////////////////////////////////////////////////////////////////
// musiconholdprocess.h
// MOH: MusicOnHold
// By yuyt, 2014/9/13.
#ifndef __MUSIC__ON__HOLD__PROCESSOR__HEADER__
#define __MUSIC__ON__HOLD__PROCESSOR__HEADER__

#ifdef IF_FEATURE_BROADSOFT_MUSIC_ON_HOLD

#include <ylhashmap.h>
#include "xmlparser/xmlparser.hpp"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

class CMusicOnHoldProcessor : public XsiBaseProcesser
{
public:
    static CMusicOnHoldProcessor & GetInstance();

    //设置MusicOnHold
    bool SetMOHState(bool bActive, int nLineID);

    //获取服务器上的MusicOnHold信息
    bool QueryMOH(int nLineID);

    //获取上次从服务器请求的MusicOnHold信息
    bool GetMOHState(int nLineID);

private:
    CMusicOnHoldProcessor(void);
    ~CMusicOnHoldProcessor(void);

    //解析XML数据
    bool ParseReponseCmdFromFile(const yl::string & strFilePath);

    //保存要提交到服务器的数据到文件
    bool SaveRequestCmdToFile(const yl::string & strFilePath);

    //override.
    virtual bool ProcessByType(XsiAction * pAction);
private:
    YLHashMap<int, bool> m_mapMOHInfo;
    bool m_bActive;
};

#define g_refMOHProcessor (CMusicOnHoldProcessor::GetInstance())

#endif //IF_FEATURE_BROADSOFT_MUSIC_ON_HOLD
#endif //__MUSIC__ON__HOLD__PROCESSOR__HEADER__
