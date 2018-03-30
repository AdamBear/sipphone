#ifndef __PROMPT_RESOLVER_ACTION_H__
#define __PROMPT_RESOLVER_ACTION_H__
#include "edkactionbase.h"

class CPromptResolverAction
    : public CEdkActionBase
{
public:
    CPromptResolverAction();
    virtual ~CPromptResolverAction();

    virtual bool DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult);

public:

    // UI响应后的回调函数
    static bool PromptUIActionCallBack(bool bOK, int iId, yl::string & strInput);

};

#endif // __PROMPT_RESOLVER_ACTION_H__
