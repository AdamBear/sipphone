#ifndef _TALK_UIPROCESSOR_LIST_H_
#define _TALK_UIPROCESSOR_LIST_H_

#include <singletonclassdefine.h>
#include <yllist.h>
#include <ylstring.h>
#include "talkuiheaders.h"
#include "basetalkuiprocessor.h"
#include "uilogicbasetalkui.h"


class CTalkUIProcessorList
{
    DECLARE_SINGLETON_CLASS(CTalkUIProcessorList)
public:
    //初始化
    bool Init();
    //反初始化
    void Uninit();

public:
    //根据SessionID 名称获取对应的逻辑处理实例
    CBaseTalkUIProcessor * GetProcessorBySessionIDAndName(int iSessionID, const yl::string & strName);
    //根据SessionID sessionState获取对应的逻辑处理实例
    CBaseTalkUIProcessor * GetProcessorBySessionIDAndSessionState(int iSessionID, int iSessionState);
    //根据传入的SessionID和Name查找对应的通话子界面逻辑处理实例
    CBaseTalkUIProcessor * FindProcessorBySessionId(int iSessionId, yl::string & strName);
    //清除与SessionID相关的逻辑处理实例
    void ClearProcessorBySessionID(int iSessionID);
    //清除所有逻辑处理实例
    void ClearAllProcessor();
    //获取保存的逻辑处理实例个数
    int GetProcessorCount();

    //把session对应的数据设置为无效  即sessionid设置为 错误的
    void SetSessionDataInvalid(int iSessionID);

    //清除错误SessionID 的 通话界面
    void ClearInvalidTalkUI();

    ////该函数用于在获取通话子窗口逻辑实例的时候，清除同ID中不需要缓存的子窗口逻辑实例
    //void CheckProcessorBySessionID(int iSessionID);

    //该函数用于在获取通话子窗口逻辑实例的时候，清除同ID中不需要缓存的子窗口逻辑实例
    //iSessionID 是要清除的sessionID， strKeepName是需要保留的窗口名称
    void CheckProcessorBySessionIDAndName(int iSessionID, const yl::string & strKeepName);

public:
    //退出所有的拨号界面， 目前只有t48、t49有callmanager模块的机型会用到该接口
    void CancelAllDialUI();

protected:
    //从缓存中获取TalkUI逻辑处理实例
    CBaseTalkUIProcessor * GetProcessorFromCache(int iSessionID, const yl::string & strName);
    //创建通话逻辑处理实例
    CBaseTalkUIProcessor * CreateProcessorByName(const yl::string & strName);
    //创建通话逻辑处理实例
    CBaseTalkUIProcessor * CreateProcessor(int iSessionID, const yl::string & strName);
    //释放通话子界面逻辑处理实例
    void ReleaseProcessor(CBaseTalkUIProcessor * pProcessor);
    //创建通话UI实例
    CUILogicBaseTalkUI * CreateTalkUI(const yl::string & strName);
    //释放通话UI实例指针
    void ReleaseTalkUI(const yl::string & strProcessorName, CUILogicBaseTalkUI * pTalkUI);
    ////SessionID 是否有效
    //bool IsSessionIDValid(int iSessionID);

protected:
    YLList<CBaseTalkUIProcessor *> m_listTalkUIProcessor;

};

// 获取全局唯一的TalkUI管理对象
#define _TalkUIProcessorList GET_SINGLETON_INSTANCE(CTalkUIProcessorList)


#endif

