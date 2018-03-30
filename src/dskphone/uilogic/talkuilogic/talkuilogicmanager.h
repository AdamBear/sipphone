#ifndef __TALK_UILOGIC_MANAGER_H__
#define __TALK_UILOGIC_MANAGER_H__

#include <singletonclassdefine.h>
#include <ylstring.h>
#include "talk/talkadapter/include/modtalkadapter.h"

// 通话回调函数
typedef void(*ON_TALK_PROC)();
typedef YLList<ON_TALK_PROC>   ListOnTalkProc;

// 对通话多路UI窗口的管理
class CTalkUILogicManager
{
    DECLARE_SINGLETON_CLASS(CTalkUILogicManager)
public:
    void Init();
    //反初始化
    void Unint();
    // 更新通话界面
    void UpdateUI(DataLogic2UI * pData);
    // session退出
    void SessionExit(int iSessionID);
    //根据通话Id清空UI对应的数据指针
    void ClearSessionData(int iSessionID);
    // 根据通话Id获得通话时间数据
    int GetTalkTimeBySessionId(int nSessionId);
    //根据Logic层所提供的数据进行错误信息更新，若有错误信息
    void UpdateErrorInfo(void * pData);
    //获取输入内容
    bool GetInputContent(yl::string & strNum, yl::string & strDisplayname);

    //刷新当前窗口
    void UpdateCurrentWnd();

    void RegisterOnTalkCallBack(ON_TALK_PROC funCallBack);

    void UnregisterOnTalkCallBack(ON_TALK_PROC funCallBack);

    void OnTalkCallBack();

    ListOnTalkProc        m_listOnTalkProc;
public:
    //退出所有的拨号界面， 目前只有t48、t49有callmanager模块的机型会用到该接口
    void CancelAllDialUI();
};

// 获取全局唯一的对象
#define _TalkUILogicManager GET_SINGLETON_INSTANCE(CTalkUILogicManager)

#endif // __TALKUILOGIC_MANAGER_H__
