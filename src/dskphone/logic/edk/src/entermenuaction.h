#ifndef __ENTER_MENU_ACTION_H__
#define __ENTER_MENU_ACTION_H__
#include "edkactionbase.h"
#include <ETLLib.hpp>

class CEnterMenuAction
    : public CEdkActionBase
{
public:
    CEnterMenuAction(void);
    virtual ~CEnterMenuAction(void);

    virtual bool DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult);

private:

    // 通过pagename映射成对应的pagetype
    int GetMenuType(yl::string & strPageName);
    // UI响应后的回调函数
    static bool MenuUIActionCallBack(bool bSuccess);
};

#endif // __ENTER_MENU_ACTION_H__
