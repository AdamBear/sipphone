#ifndef __EDK_ACTION_BASE_H__
#define __EDK_ACTION_BASE_H__
#include "../include/edk_def.h"

class CEdkActionBase
{
public:
    CEdkActionBase(EDKEXCUTETYPE eExcuteType);
    virtual ~CEdkActionBase();

    // 执行命令,返回值为是否执行下一条命令
    virtual bool DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult)
    {
        return false;
    }

    inline EDKEXCUTETYPE GetActionType()
    {
        return m_eExcuteType;
    }

    // 处理消息
    virtual bool OnMessage(msgObject & msg);

    // 解析该类型特有的内容
    virtual bool ParseAction(edk_action_data_t & tEdkAction)
    {
        return true;
    }
private:

    EDKEXCUTETYPE m_eExcuteType; // 执行类型

    //ActionCallBackFunction m_funActionCallBack; // action回调函数，通知ActionManager

};

#endif // __EDK_ACTION_BASE_H__

