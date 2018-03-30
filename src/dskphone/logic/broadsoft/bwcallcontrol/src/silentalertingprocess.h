#ifndef __BW_SILENG_ALERTING_PROCESSOR__H__
#define __BW_SILENG_ALERTING_PROCESSOR__H__

#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

class CSilentAlertingProcessor
    : public XsiBaseProcesser
{
public:
    static CSilentAlertingProcessor & GetInstance();

    //获取服务器上的信息
    bool  Query(int nLineID);

    //获取上次从服务器请求的信息
    bool  GetValue(int nLineID, bool & bValue);

    bool  Save(int nLineID, bool bEnabled);

private:
    CSilentAlertingProcessor(void);

    ~CSilentAlertingProcessor(void);

    //解析XML数据
    bool ParseReponseCmdFromFile(const yl::string & strFilePath);

    //保存数据
    bool SaveRequestCmdToFile(const yl::string & strFilePath);

    //保存服务器返回的结果
    bool Store(int nLineID);

    //override
    virtual bool ProcessByType(XsiAction * pAction);

private:
    //根据账号保存其GroupNightForward信息
    bool      *     m_pArrBValue;
    bool            m_bEnabled;
};

#define g_pSilentAlertingProcessor (CSilentAlertingProcessor::GetInstance())

#endif
