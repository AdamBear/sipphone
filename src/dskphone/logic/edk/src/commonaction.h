#ifndef __COMMON_ACTION_H__
#define __COMMON_ACTION_H__
#include "edkactionbase.h"
#include <ETLLib.hpp>

// 待定TODO
enum FUNCTIONTYPE
{

};

struct funEntryData
{
    FUNCTIONTYPE m_eFunctionType; // 功能类型
    yl::string m_strCode;         // 命令名

};

class CCommonAction
    : public CEdkActionBase
{
public:
    CCommonAction(void);
    virtual ~CCommonAction(void);

    virtual bool DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult);

private:
    // 处理定时器消息
    static LRESULT OnTimer(msgObject & objMsg) DECLARE_ATTR;
private:

};

#endif // __COMMON_ACTION_H__
