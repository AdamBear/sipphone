#ifndef __TALK_ERROR_INFO_MANAGER_H__
#define __TALK_ERROR_INFO_MANAGER_H__

#include <ylhashmap.h>
#include <ylstring.h>
#include "singletonclassdefine.h"

#ifdef IF_FEATURE_TALK_ERROR_OPT

typedef YLHashMap<int, yl::string>  TALK_ERROR_MAP;

class CTalkErrorInfoManager
{
    DECLARE_SINGLETON_CLASS(CTalkErrorInfoManager)

public:

    LRESULT Init();

public:

    LPCSTR GetErrorInfo(int iType);

private:
    static int GetErrorInfoByString(LPCSTR lpReason);

private:
    TALK_ERROR_MAP  m_mapErrorInfo;
};

#define _TalkErrorInfoManager GET_SINGLETON_INSTANCE(CTalkErrorInfoManager)

#endif
#endif // __TALK_ERROR_INFO_MANAGER_H__
