#ifndef __PROTOCOL_HELPER_H__
#define __PROTOCOL_HELPER_H__

#include "taskaction/taskactioncallbackbase.h"

using namespace NS_TA;

class CProtocolHelper : public CTaskActionCallbackBase
{
public:
    //构造函数
    CProtocolHelper();
    //析构函数
    ~CProtocolHelper();

    static CProtocolHelper & GetInstance() DECLARE_ATTR;

public:
    bool Init();

    void ReqDynamicNat();

    void ProcessNatUpdate(bool bForce = false);

    bool ParseInfo(const yl::string & strPath, yl::string & strUrl);

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    // 处理网络状态改变消息
    static LRESULT ProcessMsg(msgObject & msg);

private:
    yl::string  m_strPublicRequest; // 公网IP下载地址

    //yl::string    m_strPublicIP;      // 请求到的公网ip

    int         m_nNatType;         // Nat类型
};

#define _ProtocolHelper CProtocolHelper::GetInstance()
#endif //__PROTOCOL_HELPER_H__