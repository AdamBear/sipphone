///////////////////////////////////////////////////////////
//  CEdkManager.h
//  Implementation of the Class CEdkManager
//  Created on:      21-一月-2015 15:32:26
//  Original author: dlw
///////////////////////////////////////////////////////////

#if !defined(__EDK_MANAGER_H__)
#define __EDK_MANAGER_H__

#include "edksoftkeymanager.h"
#include <singletonclassdefine.h>
#include "ylstl/ylstring.h"
#include <ETLLib.hpp>
class CEdkManager
{
    DECLARE_SINGLETON_CLASS(CEdkManager)
public:

    // 初始化
    bool Init();
    // 执行命令流
    bool DoAction(const yl::string & strAction, int iDsskeyId = -1,
                  EDKTRIGGERTYPE eTriggerType = EDK_TRIGGER);
    // 执行命令流
    yl::string GetLabelFromAction(const yl::string & strAction, EDKACTIONTYPE eType);
    // edksoftkey的响应
    bool OnEdkSoftKeyProess(EDKSOFTKEYTYPE eType);

    // 获取softkey链表
    void GetEdkSoftKeyList(YLList<SoftKey_Data> & listSoftKey);
    // 获取softkey label
    bool GetSoftkeyLabelByType(EDKSOFTKEYTYPE eType, yl::string & strLabel);
    // 从action里面获取eType类型的数据
    yl::string GetDsskeyLabelFromAction(int iDsskeyID, EDKACTIONTYPE eType);
    // 设置提示模式开关
    bool SetTipMode(bool bEnable);
    // 判断是否为提示模式
    bool IsTipModEnable()
    {
        return m_bTipModeEnable;
    }
    // 设置action异常检测定时器
    bool SetCheckTimer(bool bEnable);
    // 定时器处理
    bool OnTimer(UINT uTimer);
    // 获取一段时间内命令执行条数
    int GetActionSize()
    {
        return m_iActionSize;
    }
    // 重置命令记录
    void ResetActionSize()
    {
        m_iActionSize = 0;
    }
    // 命令条数记录+1
    void IncreaseActionSize()
    {
        m_iActionSize ++;
    }
    // 异常检测定时器状态
    bool IsCheckTimerRuning()
    {
        return m_bCheckTimerStatus;
    }

    // 消息处理
    static LRESULT OnMessage(msgObject& objMsg);

private:
    CEdkSoftKeyManager      m_edkSoftKeyMgr;
    bool                    m_bTipModeEnable;
    int                     m_iActionSize;      // 1s内执行的命令条数
    bool
    m_bCheckTimerStatus;    // 执行异常(命令循环调用)检测定时器状态

};
// 实例
#define _EDKManager GET_SINGLETON_INSTANCE(CEdkManager)

#endif // !defined(__EDK_MANAGER_H__)
