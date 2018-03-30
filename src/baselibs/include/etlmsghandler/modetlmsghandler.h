#ifndef __MOD_ETLMSG_HANDLER_H__
#define __MOD_ETLMSG_HANDLER_H__

#include "ETLLib.hpp"

#include "msgsubscriber.h"

// 消息处理函数
typedef LRESULT(*FunctionETLMsg)(msgObject & msg);

// 注册消息辅助宏定义
#define RangeMsgReg(uMsgFrom, uMsgTo, pFun) etl_RegisterMsgHandle(uMsgFrom, uMsgTo, pFun)

// 注册消息辅助宏定义
#define SingleMsgReg(uMsg, pFun) etl_RegisterMsgHandle(uMsg, uMsg, pFun)

#define RangeMsgUnReg(uMsgFrom, uMsgTo, pFun) etl_UnregisterMsgHandle(uMsgFrom, uMsgTo, pFun)
#define SingleMsgUnReg(uMsg, pFun) etl_UnregisterMsgHandle(uMsg, uMsg, pFun)
/***********************************************************************
** 函数描述:    注册各个模块自身关注的ID消息，并绑定回调函数入口
** 参数:      [in] uMsgFrom:   消息区段开始
**             [in] uMsgTo:     消息区段结束，若只响应一条消息, 则uMsgFrom和uMsgTo的值应相等
**             [in] pFun:       函数指针，不能指向类内部的成员函数, 应该是一个全局函数或静态函数
** 返回:       TRUE，注册成功；FALSE，注册失败
*************************************************************************/
BOOL etl_RegisterMsgHandle(UINT uMsgFrom, UINT uMsgTo, FunctionETLMsg pFun);

/*******************************************************************
** 函数描述:   解注册操作，后面三个参数应与RegisterMsgHandle中传入的完全一致
** 参数:       [in] uMsgFrom:     消息区段开始
**             [in] uMsgTo:       消息区段结束
**             [in] pFun:         函数指针，不能指向类内部的成员函数, 应该是一个全局函数或静态函数
** 返回:       TRUE，解决册成功；FALSE，解注册失败
********************************************************************/
BOOL etl_UnregisterMsgHandle(UINT uMsgFrom, UINT uMsgTo, FunctionETLMsg pFun);

/*******************************************************************
** 函数描述:   消息通知函数
** 参数:       [in] bSync:              TRUE表示同步；FALSE表示异步
**             [in] uMsg:                消息ID
**             [in] wParam:              wParam
**             [in] lParam:              lParam
** 返回:       TRUE，发送成功；FALSE，发送失败
********************************************************************/
BOOL etl_NotifyApp(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*******************************************************************
** 函数描述:   带额外数据的消息通知函数
** 参数:       [in] bSync:              TRUE表示同步；FALSE表示异步
**             [in] uMsg:                消息ID
**             [in] wParam:              wParam
**             [in] lParam:              lParam
**             [in] nExtraSize:          数据大小
**             [in] lpExtraData:         数据内容
** 返回:       TRUE，发送成功；FALSE，发送失败
********************************************************************/
BOOL etl_NotifyAppEx(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam, int nExtraSize,
                     LPCVOID lpExtraData);

/*******************************************************************
** 函数描述:   消息通知请求线程函数
** 参数:       [in] bSync:              TRUE表示同步；FALSE表示异步
**             [in] DSK_NOTIFY_T:         发送的notify的链表
**             [in] uMsg:                消息ID
**             [in] wParam:              wParam
**             [in] lParam:              lParam
** 返回:       TRUE，发送成功；FALSE，发送失败
********************************************************************/
BOOL etl_NotifyReqApp(BOOL bSync, DSK_NOTIFY_T notifyChain, UINT uMsg, WPARAM wParam,
                      LPARAM lParam);

/*******************************************************************
** 函数描述:   带额外数据的消息通知请求线程函数
** 参数:       [in] bSync:              TRUE表示同步；FALSE表示异步
**             [in] DSK_NOTIFY_T:         发送的notify的链表
**             [in] uMsg:                消息ID
**             [in] wParam:              wParam
**             [in] lParam:              lParam
**             [in] nExtraSize:          数据大小
**             [in] lpExtraData:         数据内容
** 返回:       TRUE，发送成功；FALSE，发送失败
********************************************************************/
BOOL etl_NotifyReqAppEx(BOOL bSync, DSK_NOTIFY_T notifyChain, UINT uMsg, WPARAM wParam,
                        LPARAM lParam, int nExtraSize, LPCVOID lpExtraData);

//ETL消息处理
BOOL etl_OnMessageProcess(msgObject & objMessage);

/**
 * @brief Etl dump message map.
 *
 * @author Song
 * @date 2017/9/4
 */

void etl_DumpMsgMap(int iMessage = -1);

// 构造subscriber的参数可以是全局函数也可以是类成员函数，示例如下
// etl_RegisterMsgHandle(SIP_CALL_TRANSFERED_RESULT, SIP_CALL_SEND_INFO_RESULT, CMSGSubscriber(talk_ProcessSIPMsg));
// etl_RegisterMsgHandle(SIP_CALL_TRANSFERED_RESULT, SIP_CALL_SEND_INFO_RESULT, CMSGSubscriber(this, &CTalklogicManager::OnSIPMsg));
// 成员函数类型：typedef bool (T::*MemberFunctionType)(msgObject&);
// warning：类成员函数类型必须在析构时调用对应的etl_UnregisterMsgHandle
BOOL etl_RegisterMsgHandle(UINT uMsgFrom, UINT uMsgTo, const CMSGSubscriber & subscriber);

// subscriber参数需要和etl_RegisterMsgHandle相同
BOOL etl_UnregisterMsgHandle(UINT uMsgFrom, UINT uMsgTo, const CMSGSubscriber & subscriber);
//RAII support
class CMSGRegisterScopeGuard
{
public:
    CMSGRegisterScopeGuard(UINT uMsgFrom, UINT uMsgTo, const CMSGSubscriber & subscriber)
        : m_msgFrom(uMsgFrom), m_msgTo(uMsgTo), m_subscriber(subscriber)
    {
        DoRegister();
    };
    CMSGRegisterScopeGuard(UINT uMsg, const CMSGSubscriber & subscriber)
        : m_msgFrom(uMsg), m_msgTo(uMsg), m_subscriber(subscriber)
    {
        DoRegister();
    };
    ~CMSGRegisterScopeGuard()
    {
        DoUnRegister();
    };

private:
    CMSGRegisterScopeGuard(const CMSGRegisterScopeGuard &);
    CMSGRegisterScopeGuard & operator=(const CMSGRegisterScopeGuard &);

    void DoRegister()
    {
        etl_RegisterMsgHandle(m_msgFrom, m_msgTo, m_subscriber);
    }
    void DoUnRegister()
    {
        etl_UnregisterMsgHandle(m_msgFrom, m_msgTo, m_subscriber);
    }

    const UINT m_msgFrom;
    const UINT m_msgTo;
    const CMSGSubscriber m_subscriber;
};


#endif // __MOD_ETLMSG_HANDLER_H__
