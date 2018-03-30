#ifndef __CLID_PROCESSER_H__
#define __CLID_PROCESSER_H__

#include <ylhashmap.h>
#include "xmlparser/xmlparser.hpp"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

class CLIDProcesser : public XsiBaseProcesser
{
public:

    static CLIDProcesser & GetInstance();

    //开启或者关闭匿名呼叫
    bool SetAnonymousCallState(bool bAnonymousCallState, int nLineID);

    //查询匿名呼叫状态
    bool QueryAnonymousCallState(int nLineID);

    //获取保存的Anonymous Call状态
    bool GetAnonymousCallState(int nLineID);

    //开启或者关闭匿名呼叫拒绝
    bool SetAnonymousCallRejectionState(bool bAnonymousCallRejectionState, int nLineID);

    //查询匿名呼叫拒绝状态
    bool QueryAnonymousCallRejectionState(int nLineID);

    //获取保存的Anonymous Call Rejection状态
    bool GetAnonymousCallRejectionState(int nLineID);

private:

    CLIDProcesser(void);

    virtual ~CLIDProcesser(void);

    bool SetCallState(eXsiActionType eType, bool bActive, int nLineID);

    //解析XML数据
    bool ParseReponseCmdFromFile(const yl::string & strFilePath);

    //保存要提交到服务器的数据到文件,如果命令类型不需要发送内容时返回true
    bool SaveDataToFile(bool bActive, const yl::string & strFilePath, eXsiActionType eType);

protected:

    virtual bool ProcessByType(XsiAction * pAction);

private:

    //根据账号保存其Anonymous Call状态
    YLHashMap<int, bool> m_mapAnonymousCall;

    //根据账号保存其Anonymous Call Rejection状态
    YLHashMap<int, bool> m_mapAnonymousCallRejection;

    bool m_bTempState;
};

#define _CLIDProcesser CLIDProcesser::GetInstance()


#endif

