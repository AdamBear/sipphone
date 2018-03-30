#ifndef __CFORWARD_SELECTIVE_PROCESSER_H__
#define __CFORWARD_SELECTIVE_PROCESSER_H__

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE

#include <ylhashmap.h>
#include "xmlparser/xmlparser.hpp"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"


class CForwardSelectiveProcesser : public XsiBaseProcesser
{
public:

    static CForwardSelectiveProcesser & GetInstance();

    // 保存
    bool SetForwardSelective(const SForwardSelectiveData & sData, int nLineID);
    // 获取
    bool GetForwardSelective(SForwardSelectiveData & sData, int nLineID);
    // 查询
    bool QueryForwardSelective(int nLineID);

private:

    CForwardSelectiveProcesser(void);

    virtual ~CForwardSelectiveProcesser(void);

    //解析XML数据
    bool ParseReponseCmdFromFile(const yl::string & strFilePath);

    //保存要提交到服务器的数据到文件,如果命令类型不需要发送内容时返回true
    bool SaveRequestCmdToFile(const yl::string & strFilePath);

protected:

    virtual bool ProcessByType(XsiAction * pAction);

private:

    typedef YLHashMap<int, SForwardSelectiveData> mapFSData;
    //根据账号保存其ForwardSelective状态
    mapFSData               m_mapForwardSelective;
    SForwardSelectiveData   m_FSItem;
};

#define _CForwardSelectiveProcesser CForwardSelectiveProcesser::GetInstance()
#endif

#endif //__CFORWARD_SELECTIVE_PROCESSER_H__

