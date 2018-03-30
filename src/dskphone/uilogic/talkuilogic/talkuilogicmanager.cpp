#include "talkuilogicmanager.h"
#include "talkuilogic_inc.h"
#include "talkuilogicframemanager.h"

IMPLEMENT_GETINSTANCE(CTalkUILogicManager)

CTalkUILogicManager::CTalkUILogicManager()
{

}

CTalkUILogicManager::~CTalkUILogicManager()
{

}

void CTalkUILogicManager::Init()
{
    _TalkUIProcessorList.Init();
    _TalkUILogicFrameManager.Init();
}

//反初始化
void CTalkUILogicManager::Unint()
{
    _TalkUILogicFrameManager.Uninit();
    _TalkUIProcessorList.Uninit();
}

void CTalkUILogicManager::UpdateUI(DataLogic2UI * pData)
{
    OnTalkCallBack();
    _TalkUILogicFrameManager.UpdateUI(pData);
}

void CTalkUILogicManager::SessionExit(int iSessionID)
{
    _TalkUILogicFrameManager.SessionExit(iSessionID);
}

//根据通话Id清空UI对应的数据指针
void CTalkUILogicManager::ClearSessionData(int iSessionID)
{
    _TalkUILogicFrameManager.ClearSessionData(iSessionID);
}

// 功能  ： 根据通话Id获得通话时间数据
int CTalkUILogicManager::GetTalkTimeBySessionId(int nSessionId)
{
    return _TalkUILogicFrameManager.GetTalkTimeBySessionId(nSessionId);
}

//根据Logic层所提供的数据进行错误信息更新，若有错误信息
void CTalkUILogicManager::UpdateErrorInfo(void * pData)
{
    _TalkUILogicFrameManager.UpdateErrorInfo(pData);
}

bool CTalkUILogicManager::GetInputContent(yl::string & strNum, yl::string & strDisplayname)
{
    return _TalkUILogicFrameManager.GetInputContent(strNum, strDisplayname);
}

//刷新当前窗口
void CTalkUILogicManager::UpdateCurrentWnd()
{
    _TalkUILogicFrameManager.UpdateCurrentWnd();
}

//退出所有的拨号界面， 目前只有t48、t49有callmanager模块的机型会用到该接口
void CTalkUILogicManager::CancelAllDialUI()
{
    _TalkUIProcessorList.CancelAllDialUI();
}

void CTalkUILogicManager::RegisterOnTalkCallBack(ON_TALK_PROC funCallBack)
{
    ListOnTalkProc::iterator it = yl::find(m_listOnTalkProc.begin(),
                                           m_listOnTalkProc.end(), funCallBack);

    if (it == m_listOnTalkProc.end())
    {
        m_listOnTalkProc.push_back(funCallBack);
    }
}

void CTalkUILogicManager::UnregisterOnTalkCallBack(ON_TALK_PROC funCallBack)
{
    ListOnTalkProc::iterator it = yl::find(m_listOnTalkProc.begin(),
                                           m_listOnTalkProc.end(), funCallBack);

    if (it != m_listOnTalkProc.end())
    {
        m_listOnTalkProc.erase(it);
    }
}

void CTalkUILogicManager::OnTalkCallBack()
{
    ListOnTalkProc::iterator it = m_listOnTalkProc.begin();
    for (; it != m_listOnTalkProc.end(); ++it)
    {
        if ((*it) != NULL)
        {
            (*it)();
        }
    }
}
