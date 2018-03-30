#ifndef __HARDKEY_ACTION_H__
#define __HARDKEY_ACTION_H__

#include "edkactionbase.h"
#include <ETLLib.hpp>

typedef YLHashMap<yl::string, int> typeMapKeyStringToId;
typedef typeMapKeyStringToId::iterator ITER_STRING_TO_ID_MAP;

class CKeyAction
    : public CEdkActionBase
{
public:
    CKeyAction(void);
    virtual ~CKeyAction(void);

    virtual bool DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult);

private:
    // 执行硬按键
    bool ExcuteHardKey(int iActionId, edk_action_data_t & tEdkAction, bool & bResult);
    // 执行softkey
    bool ExcuteSoftKey(edk_action_data_t & tEdkAction);
    // UI响应后的回调函数
    static bool SoftkeyUIActionCallBack(bool bSuccess);

    void SetTimer(int iActionId);
    // 通过action获取dsskey id
    int GetDsskeyIDByAction(const yl::string & strKey);
    //
    bool HandleVolKey(const yl::string & strKey);
private:
    typeMapKeyStringToId m_mapKeyStringToId;
};

#endif // __HARDKEY_ACTION_H__

