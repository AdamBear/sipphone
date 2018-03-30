#if IF_FEATURE_METASWITCH
#ifndef _MTSWACTIONCMDPARSERBASE_H_
#define _MTSWACTIONCMDPARSERBASE_H_

#include "xmlparser/xmlparser.hpp"
#include "threadtask/include/common.h"
#include <ETLLib.hpp>
#include "mtswaction.h"

class CMTSWActionCmdBase
{
public:
    CMTSWActionCmdBase()    {}

    virtual ~CMTSWActionCmdBase() {}

    //同步失败处理
    virtual void OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType, int nId) = 0;

    //同步返回
    virtual bool OnLoadSucProcess(CMTSWAction * pAction)
    {
        return false;
    }

protected:
    inline void GetFailCodeString(yl::string & strFail, NS_TA::TaskActionErrorType eErrCode);
};

inline void CMTSWActionCmdBase::GetFailCodeString(yl::string & strFail,
        NS_TA::TaskActionErrorType eErrCode)
{
    switch (eErrCode)
    {
    case NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED:

        strFail = "auth error";
        break;
    case NS_TA::TA_HTTP_ERR400:

        strFail = "Bad Request";
        break;
    case NS_TA::TA_HTTP_ERR403:

        strFail = "Forbidden";
        break;
    case NS_TA::TA_HTTP_ERR408:

        strFail = "Time out";
        break;
    case NS_TA::TA_HTTP_ERR500:

        strFail = "server error";
        break;
    default:
        strFail = "Unknown";
        break;
    }
}

#endif //_MTSWACTIONCMDPARSERBASE_H_
#endif

