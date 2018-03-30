//#include "../ui/talkui_inc.h"
#include "talkuiprocessorlist.h"
#include "talkuilogic_inc.h"
#include "basetalkuiprocessor.h"

//通话
#include "talkingprocessor.h"
#include "ringprocessor.h"
#include "connectprocessor.h"
#include "finishprocessor.h"
#include "dialprocessor.h"
#include "confprocessor.h"
#include "confmanagerprocessor.h"
#include "parknotifyprocessor.h"
#include "autoredialprocessor.h"
#include "rtpstatusprocessor.h"
#include "acdcallinfoprocessor.h"
#include "netconfprocessor.h"
#include "netconfmgrprocessor.h"

//错误的sessionID  清除数据的时候 会设置
#define WRONG_SESSIONID -2


IMPLEMENT_GETINSTANCE(CTalkUIProcessorList)

CTalkUIProcessorList::CTalkUIProcessorList()
{

}

CTalkUIProcessorList::~CTalkUIProcessorList()
{

}

//初始化
bool CTalkUIProcessorList::Init()
{
    return true;
}

//反初始化
void CTalkUIProcessorList::Uninit()
{
    ClearAllProcessor();
}

//根据SessionID 和 名称获取对应的逻辑处理实例
CBaseTalkUIProcessor * CTalkUIProcessorList::GetProcessorBySessionIDAndName(int iSessionID,
        const yl::string & strName)
{
    //如果SessionID非法 那么返回NULL
    if (false == IsSessionIDValid(iSessionID))
    {
        return NULL;
    }

    //如果UI名称为空 那么返回NULL
    if (true == strName.empty())
    {
        return NULL;
    }

    CBaseTalkUIProcessor * pProcessor = NULL;
    //获取缓存的 通话子界面逻辑处理实例
    pProcessor = GetProcessorFromCache(iSessionID, strName);

    //从缓存中有获取到对应的实例 则直接返回 不需要删除了
    if (NULL != pProcessor)
    {
        return pProcessor;
    }

    //该函数内部赋值SessionID构造函数的时候自动把名称赋值
    pProcessor = CreateProcessor(iSessionID, strName);

    //创建不成功 直接返回NULL
    if (NULL == pProcessor)
    {
        return NULL;
    }

    ////需要先删除之前需要删除的界面  然后才把当前对话框入栈
    //CheckProcessorBySessionID(iSessionID);

    //把该对话框实例插入到缓存列表中
    m_listTalkUIProcessor.push_back(pProcessor);

    return pProcessor;
}

//根据SessionID sessionState获取对应的逻辑处理实例
CBaseTalkUIProcessor * CTalkUIProcessorList::GetProcessorBySessionIDAndSessionState(int iSessionID,
        int iSessionState)
{
    //如果SessionID非法 那么返回NULL
    if (false == IsSessionIDValid(iSessionID))
    {
        return NULL;
    }

    CBaseTalkUIProcessor * pFindProcessor = NULL;
    //查找TalkUI
    YLList<CBaseTalkUIProcessor *>::const_iterator itFind = m_listTalkUIProcessor.begin();

    for (; itFind != m_listTalkUIProcessor.end(); ++itFind)
    {
        CBaseTalkUIProcessor * pProcessor = (CBaseTalkUIProcessor *)*itFind;

        if (NULL != pProcessor
                && pProcessor->GetSessionID() == iSessionID)
        {
            if (pProcessor->GetSessionState() == iSessionState)
            {
                return pProcessor;
            }
        }
    }

    //返回对应指针
    return pFindProcessor;
}


//根据传入的SessionID和Name查找对应的通话子界面逻辑处理实例
CBaseTalkUIProcessor * CTalkUIProcessorList::FindProcessorBySessionId(int iSessionID,
        yl::string & strName)
{
    //如果SessionID非法 那么返回NULL
    if (false == IsSessionIDValid(iSessionID))
    {
        return NULL;
    }

    CBaseTalkUIProcessor * pFindProcessor = NULL;
    //查找TalkUI
    YLList<CBaseTalkUIProcessor *>::const_iterator itFind = m_listTalkUIProcessor.begin();

    for (; itFind != m_listTalkUIProcessor.end(); ++itFind)
    {
        CBaseTalkUIProcessor * pProcessor = (CBaseTalkUIProcessor *)*itFind;

        if (NULL != pProcessor
                && pProcessor->GetSessionID() == iSessionID)
        {
            if ("" == strName
                    || pProcessor->GetProcessorName() == strName)
            {
                //查找到对应SessionId的TalkUI指针
                return pProcessor;
            }

            if (pFindProcessor == NULL)
            {
                pFindProcessor = pProcessor;
            }
        }
    }

    //返回对应指针
    return pFindProcessor;
}

//清除与SessionID相关的逻辑处理实例
void CTalkUIProcessorList::ClearProcessorBySessionID(int iSessionID)
{
    TALKUI_INFO("CTalkUIProcessorList::ClearProcessorBySessionID begin  sessionID=%d", iSessionID);
    //ID非法也需要清除
    ////如果SessionID非法 那么返回NULL
    //if(false == IsSessionIDValid(iSessionID))
    //{
    //  return ;
    //}

    YLList<CBaseTalkUIProcessor *>::iterator iter = m_listTalkUIProcessor.begin();
    //遍历 TalkUI列表查找 DlgBaseTalkUI
    for (; m_listTalkUIProcessor.end() != iter;)
    {
        CBaseTalkUIProcessor * pProcessor = *iter;
        if (NULL != pProcessor)
        {
            if (pProcessor->GetSessionID() == iSessionID)
            {
                ReleaseProcessor(pProcessor);
                YLList<CBaseTalkUIProcessor *>::iterator iterNext = iter;
                iterNext++;
                m_listTalkUIProcessor.erase(iter);
                iter = iterNext;
                continue;
            }
        }
        iter++;
    }

    TALKUI_INFO("CTalkUIProcessorList::ClearProcessorBySessionID end sessionID=%d", iSessionID);
}

//清除所有逻辑处理实例
void CTalkUIProcessorList::ClearAllProcessor()
{
    TALKUI_INFO("CTalkUIProcessorList::ClearAllProcessor begin");

    YLList<CBaseTalkUIProcessor *>::iterator iter = m_listTalkUIProcessor.begin();
    for (; iter != m_listTalkUIProcessor.end(); ++iter)
    {
        CBaseTalkUIProcessor * pProcessor = *iter;
        ReleaseProcessor(pProcessor);
    }
    m_listTalkUIProcessor.clear();

    TALKUI_INFO("CTalkUIProcessorList::ClearAllProcessor end");
}

//获取保存的逻辑处理实例个数
int CTalkUIProcessorList::GetProcessorCount()
{
    return m_listTalkUIProcessor.size();
}

//把session对应的数据设置为无效  即sessionid设置为 错误的
void CTalkUIProcessorList::SetSessionDataInvalid(int iSessionID)
{
    TALKUI_INFO(" CTalkUIProcessorList::SetSessionDataInvalid sessionID=%d", iSessionID);

    //如果本身sessionID已经是无效的了 那么就不需要浪费效率了
    //http://bugfree.yealink.com/Bug.php?BugID=92578 之前的逻辑层错误 改为!IsSessionIDValid(iSessionID)
    if (!IsSessionIDValid(iSessionID))
    {
        TALKUI_INFO(" CTalkUIProcessorList::SetSessionDataInvalid end sessionID=%d", iSessionID);
        return ;
    }

    YLList<CBaseTalkUIProcessor *>::iterator iteList = m_listTalkUIProcessor.begin();

    //遍历 TalkUI列表查找 DlgBaseTalkUI
    for (; m_listTalkUIProcessor.end() != iteList;)
    {
        CBaseTalkUIProcessor * pTalkUIProcessor = *iteList;

        //做容错判断
        if (NULL == pTalkUIProcessor)
        {
            YLList<CBaseTalkUIProcessor *>::iterator iteListNext = iteList;
            iteListNext++;
            m_listTalkUIProcessor.erase(iteList);
            iteList = iteListNext;
            continue;
        }

        //设置 sessionID 为错误的
        if (pTalkUIProcessor->GetSessionID() == iSessionID)
        {
            TALKUI_INFO(" CTalkUIProcessorList::SetSessionDataInvalid set wrongid name=%s", pTalkUIProcessor->GetProcessorName().c_str());

            pTalkUIProcessor->SetSessionID(WRONG_SESSIONID);
        }

        iteList++;
    }

    TALKUI_INFO(" CTalkUIProcessorList::SetSessionDataInvalid end sessionID=%d", iSessionID);
}

//清除错误SessionID 的 通话界面
void CTalkUIProcessorList::ClearInvalidTalkUI()
{
    TALKUI_INFO("CTalkUIProcessorList::ClearInvalidTalkUI begin");

    ClearProcessorBySessionID(WRONG_SESSIONID);

    TALKUI_INFO("CTalkUIProcessorList::ClearInvalidTalkUI end");
}


////该函数用于在获取通话子窗口逻辑实例的时候，清除同ID中不需要缓存的子窗口逻辑实例
//void CTalkUIProcessorList::CheckProcessorBySessionID(int iSessionID)
//{
//  //如果SessionID非法 那么返回NULL
//  //if(false == IsSessionIDValid(iSessionID))
//  //{
//  //  return;
//  //}
//
//  YLList<CBaseTalkUIProcessor*>::iterator iter = m_listTalkUIProcessor.begin();
//  //遍历 TalkUI列表查找 DlgBaseTalkUI
//  for(; m_listTalkUIProcessor.end() != iter;)
//  {
//      CBaseTalkUIProcessor* pProcessor = *iter;
//      if (NULL != pProcessor)
//      {
//          if(pProcessor->GetSessionID() == iSessionID)
//          {
//              yl::string strProcessorName = pProcessor->GetProcessorName();
//              //只有 Talking、Conf、ConfManager、ParkNotify 等需要界面保存数据的 才需要缓存
//              if( (PROCESSORNAME_TALKING != strProcessorName)
//                  && (PROCESSORNAME_CONF != strProcessorName)
//                  && (PROCESSORNAME_CONFMANAGER != strProcessorName)
//                  && (PROCESSORNAME_PARKNOTIFY != strProcessorName)       //parknotify 也是需要保存时间的
//                  )
//              {
//                  ReleaseProcessor(pProcessor);
//                  YLList<CBaseTalkUIProcessor*>::iterator iterNext = iter;
//                  iterNext++;
//                  m_listTalkUIProcessor.erase(iter);
//                  iter = iterNext;
//                  continue;
//              }
//          }
//      }//NULL != pProcessor
//      iter++;
//  }
//}

//该函数用于在获取通话子窗口逻辑实例的时候，清除同ID中不需要缓存的子窗口逻辑实例
void CTalkUIProcessorList::CheckProcessorBySessionIDAndName(int iSessionID,
        const yl::string & strKeepName)
{
    TALKUI_INFO("CTalkUIProcessorList::CheckProcessorBySessionIDAndName begin");
    //如果SessionID非法 那么返回NULL
    //if(false == IsSessionIDValid(iSessionID))
    //{
    //  return;
    //}

    YLList<CBaseTalkUIProcessor *>::iterator iter = m_listTalkUIProcessor.begin();
    //遍历 TalkUI列表查找 DlgBaseTalkUI
    for (; m_listTalkUIProcessor.end() != iter;)
    {
        CBaseTalkUIProcessor * pProcessor = *iter;
        if (NULL != pProcessor)
        {
            if (pProcessor->GetSessionID() == iSessionID)
            {
                yl::string strProcessorName = pProcessor->GetProcessorName();
                //只有 Talking、Conf、ConfManager、ParkNotify 等需要界面保存数据的 才需要缓存
                //这边不能加上拨号界面 ， 在获取时间的时候 拨号界面是没有时间的 会出现问题
                if ((PROCESSORNAME_TALKING != strProcessorName)
                        && (PROCESSORNAME_CONF != strProcessorName)
                        && (PROCESSORNAME_CONFMANAGER != strProcessorName)
                        && (PROCESSORNAME_NETCONF != strProcessorName)
                        && (PROCESSORNAME_NETCONFMGR != strProcessorName)
                        && (PROCESSORNAME_PARKNOTIFY != strProcessorName)       //parknotify 也是需要保存时间的
                        && ((strKeepName.empty())
                            || (strKeepName != strProcessorName))                      //要保留的也不能删除
                   )
                {
                    TALKUI_INFO("CTalkUIProcessorList::CheckProcessorBySessionIDAndName sessionID=%d, ProcessorName=%s",
                                pProcessor->GetSessionID(), strProcessorName.c_str());

                    ReleaseProcessor(pProcessor);
                    YLList<CBaseTalkUIProcessor *>::iterator iterNext = iter;
                    iterNext++;
                    m_listTalkUIProcessor.erase(iter);
                    iter = iterNext;
                    continue;
                }
            }
        }//NULL != pProcessor
        iter++;
    }

    TALKUI_INFO("CTalkUIProcessorList::CheckProcessorBySessionIDAndName end");
}

//退出所有的拨号界面， 目前只有t48、t49有callmanager模块的机型会用到该接口
void CTalkUIProcessorList::CancelAllDialUI()
{
    YLList<CBaseTalkUIProcessor *>::iterator iter = m_listTalkUIProcessor.begin();
    //遍历 列表查找 逻辑类
    for (; m_listTalkUIProcessor.end() != iter; iter++)
    {
        CBaseTalkUIProcessor * pProcessor = *iter;
        if (NULL != pProcessor)
        {
            //当前是拨号界面
            if (PROCESSORNAME_DIAL == pProcessor->GetProcessorName())
            {
                DataLogic2UI objDataLogic2UI;

                //SessionID有效那么才要发cancel
                if (IsSessionIDValid(pProcessor->GetSessionID()))
                {
                    //从逻辑层获取数据
                    talklogic_GetDataBySessionID(pProcessor->GetSessionID(), objDataLogic2UI);

                    DataLogic2UI objDataLogic2UINew;
                    //根据数据获取对话框名称
                    yl::string strProcessorName = GetProcessorNameFromData(&objDataLogic2UI, objDataLogic2UINew);
                    //如果当前还是拨号界面的那么才发送 cancel
                    if (PROCESSORNAME_DIAL == strProcessorName)
                    {
                        //发送取消事件
                        pProcessor->DealSoftkeyPress(ST_CANCEL);
                        //  pProcessor->SendUIRequestAction(TALK_ACTION_CANCEL);
                    }
                }

            }


        }
    }


}

//从缓存中获取TalkUI逻辑处理实例
CBaseTalkUIProcessor * CTalkUIProcessorList::GetProcessorFromCache(int iSessionID,
        const yl::string & strName)
{
    //如果SessionID非法 那么返回NULL
    if (false == IsSessionIDValid(iSessionID))
    {
        return NULL;
    }

    //如果UI名称为空 那么返回NULL
    if (true == strName.empty())
    {
        return NULL;
    }

    YLList<CBaseTalkUIProcessor *>::iterator iter = m_listTalkUIProcessor.begin();
    //遍历 TalkUI列表查找 DlgBaseTalkUI
    for (; m_listTalkUIProcessor.end() != iter; ++iter)
    {
        CBaseTalkUIProcessor * pProcessor = *iter;
        if (NULL != pProcessor)
        {
            if ((pProcessor->GetSessionID() == iSessionID)
                    && (pProcessor->GetProcessorName() == strName))
            {
                //根据名称获取通话UI子界面实例
                CUILogicBaseTalkUI * pTalkUI = CreateTalkUI(strName);
                TALKUI_INFO("CBaseTalkUI* pTalkUI = CreateTalkUI pTalkUI=0x%x", pTalkUI);

                if (NULL == pTalkUI)
                {
                    return NULL;
                }

                //绑定sessionID
                pProcessor->SetSessionID(iSessionID);

                //给Processor 绑定 通话UI
                pProcessor->SetBindUI(pTalkUI);

                //给通话UI 绑定   Processor
                pTalkUI->BindProcessor(pProcessor);

                return pProcessor;
            }
        }
    }

    return NULL;
}


//创建通话逻辑处理实例
CBaseTalkUIProcessor * CTalkUIProcessorList::CreateProcessor(int iSessionID,
        const yl::string & strName)
{
    if (false == IsSessionIDValid(iSessionID))
    {
        return NULL;
    }

    if (0 >= strName.length())
    {
        return NULL;
    }

    //根据名称获取通话UI子界面实例
    CUILogicBaseTalkUI * pTalkUI = CreateTalkUI(strName);

    TALKUI_INFO("CBaseTalkUI* pTalkUI = CreateTalkUI pTalkUI=0x%x", pTalkUI);

    if (NULL == pTalkUI)
    {
        return NULL;
    }

    //生成 Processor 实例
    CBaseTalkUIProcessor * pProcessor = CreateProcessorByName(strName);

    //如果获取到的 processor 实例为空
    if (NULL == pProcessor)
    {
        //释放通话UI 实例   防止内存泄露
        ReleaseTalkUI(strName, pTalkUI);
        pTalkUI = NULL;
        return NULL;
    }

    //绑定sessionID
    pProcessor->SetSessionID(iSessionID);

    //给Processor 绑定 通话UI
    pProcessor->SetBindUI(pTalkUI);

    //给通话UI 绑定   Processor
    pTalkUI->BindProcessor(pProcessor);

    //调用初始化
    pProcessor->Init();

    TALKUI_INFO("---------CREATE Processor sessionID=%d, Processor=0x%x", iSessionID, pProcessor);

    return pProcessor;
}

CBaseTalkUIProcessor * CTalkUIProcessorList::CreateProcessorByName(const yl::string & strName)
{
    CBaseTalkUIProcessor * pTalkUIProcessor = NULL;
    if ("" == strName)
    {
        return NULL;
    }
    if (PROCESSORNAME_ACDCALLINFO == strName)
    {
        pTalkUIProcessor = new CACDCallInfoProcessor();
    }
    else if (PROCESSORNAME_AUTOREDIAL == strName)
    {
        pTalkUIProcessor = new CAutoRedialProcessor();
    }
    else if (PROCESSORNAME_CONF == strName)
    {
        pTalkUIProcessor = new CConfProcessor();
    }
    else if (PROCESSORNAME_CONFMANAGER == strName)
    {
        pTalkUIProcessor = new CConfManagerProcessor();
    }
    else if (PROCESSORNAME_CONNECT == strName)
    {
        pTalkUIProcessor = new CConnectProcessor();
    }
    else if (PROCESSORNAME_DIAL == strName)
    {
        pTalkUIProcessor = new CDialProcessor();
    }
    else if (PROCESSORNAME_FINISH == strName)
    {
        pTalkUIProcessor = new CFinishProcessor();
    }
    else if (PROCESSORNAME_PARKNOTIFY == strName)
    {
        pTalkUIProcessor = new CParkNotifyProcessor();
    }
    else if (PROCESSORNAME_RING == strName)
    {
        pTalkUIProcessor = new CRingProcessor();
    }
    else if (PROCESSORNAME_RTPSTATUS == strName)
    {
        pTalkUIProcessor = new CRTPStatusProcessor();
    }
    else if (PROCESSORNAME_TALKING == strName)
    {
        pTalkUIProcessor = new CTalkingProcessor();
    }
    else if (PROCESSORNAME_NETCONF == strName)
    {
        pTalkUIProcessor = new CNetConfProcessor();
    }
    else if (PROCESSORNAME_NETCONFMGR == strName)
    {
        pTalkUIProcessor = new CNetConfMgrProcessor();
    }

    return pTalkUIProcessor;
}

//释放通话子界面逻辑处理实例
void CTalkUIProcessorList::ReleaseProcessor(CBaseTalkUIProcessor * pProcessor)
{
    if (NULL == pProcessor)
    {
        return ;
    }
    //解绑前调用  反初始化
    pProcessor->Uninit();

    //获取绑定UI
    CUILogicBaseTalkUI * pTalkUI = pProcessor->GetBindUI();

    //设置绑定的UI 为空
    pProcessor->SetBindUI(NULL);
    TALKUI_INFO("pProcessor->SetBindUI(NULL)  pProcessor=0x%x", pProcessor);

    //设置通话子界面绑定的 逻辑处理类为空
    if (NULL != pTalkUI)
    {
        pTalkUI->BindProcessor(NULL);
        TALKUI_INFO("pTalkUI->BindProcessor(NULL)  pTalkUI=0x%x", pTalkUI);

        //释放通话UI  实例
        yl::string strProcessorName = pProcessor->GetProcessorName();
        ReleaseTalkUI(strProcessorName, pTalkUI);

        pTalkUI = NULL;
    }

    //删除 通话逻辑 实例
    pProcessor->deleteLater();

}

CUILogicBaseTalkUI * CTalkUIProcessorList::CreateTalkUI(const yl::string & strName)
{
    return _TalkUILogicFrameManager.CreateTalkUI(strName);
}

void CTalkUIProcessorList::ReleaseTalkUI(const yl::string & strProcessorName,
        CUILogicBaseTalkUI * pTalkUI)
{
    TALKUI_INFO("CTalkUIProcessorList::ReleaseTalkUI( pTalkUI=0x%x )", pTalkUI);
    _TalkUILogicFrameManager.ReleaseTalkUI(strProcessorName, pTalkUI);
}

////SessionID 是否有效
//bool CTalkUIProcessorList::IsSessionIDValid(int iSessionID)
//{
//  if(iSessionID < 0)
//  {
//      return false;
//  }
//  return true;
//}

