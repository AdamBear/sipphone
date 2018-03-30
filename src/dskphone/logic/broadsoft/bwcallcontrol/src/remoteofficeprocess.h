//////////////////////////////////////////////////////////////////////////
//  remoteofficeprocess.h
//  By yuyt! 2014/8/15
#ifndef __REMOTE__OFFICE__PROCESSOR__HEADER__
#define __REMOTE__OFFICE__PROCESSOR__HEADER__

#include <ylhashmap.h>
#include "xmlparser/xmlparser.hpp"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

class CRemoteOfficeProcessor
    : public XsiBaseProcesser
{
public:
    static CRemoteOfficeProcessor & GetInstance();

    //设置Remote Office
    bool SetRemoteOffice(bool bState, const char * szRemoteNumber, int nLineID);

    //获取服务器上的Remote Office信息，返回的结果是通过发送RO_MESSAGE_GET_RESULT消息来传送的
    bool QueryRemoteOffice(int nLineID);

    //获取上次从服务器请求的Remote office信息
    void GetRemoteOfficeInfo(SRemoteOffice & roInfo, int nLineID);

private:
    CRemoteOfficeProcessor(void);
    ~CRemoteOfficeProcessor(void);

    //override,处理cmd执行之后服务器返回的数据
    virtual bool ProcessByType(XsiAction * pAction);

    //保存服务器返回的结果
    void SaveRemoteOffice(const SRemoteOffice & sRO, int nLineID);

    //解析XML数据
    bool ParseReponseCmdFromFile(const yl::string & strFilePath);

    //保存要提交到服务器的数据到文件,如果命令类型不需要发送内容时返回true
    bool SaveRequestCmdToFile(const yl::string & strFilePath);

private:
    //根据账号保存其Remote Office信息
    YLHashMap<int, SRemoteOffice> m_mapRemoteOffice;

    SRemoteOffice m_sRO;
};

#define g_refRemoteOfficeProcessor (CRemoteOfficeProcessor::GetInstance())

#endif//__REMOTE__OFFICE__PROCESSOR__HEADER__
