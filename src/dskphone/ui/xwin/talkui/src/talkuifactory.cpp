#include "talkuifactory.h"

#include "talkuilogic/uilogicbasetalkui.h"

namespace talkui
{

CTalkUIFactory::CTalkUIFactory()
    : m_pACDCallInfoUI(NULL)
    , m_pAutoRedialUI(NULL)
    , m_pConfUI(NULL)
    , m_pConfManagerUI(NULL)
    , m_pConnectUI(NULL)
    , m_pDialingUI(NULL)
    , m_pFinishUI(NULL)
    , m_pParkNotifyUI(NULL)
    , m_pRingUI(NULL)
    , m_pRTPStatusUI(NULL)
    , m_pTalkingUI(NULL)
    , m_pNetConfUI(NULL)
    , m_pNetConfMgrUI(NULL)
{
}

CTalkUIFactory::~CTalkUIFactory()
{
}

void CTalkUIFactory::init()
{
    //拨号界面先创建出来
    m_pDialingUI = new CDlgDialingUI();
}

CTalkUIFactory & CTalkUIFactory::GetInstance()
{
    static CTalkUIFactory s_objTalkUIFactory;

    return s_objTalkUIFactory;
}

CUILogicBaseTalkUI * CTalkUIFactory::CreateTalkUI(const yl::string & strName)
{
    return GetTalkUIFromCache(strName, true);
}

CDlgBaseTalkUI * CTalkUIFactory::GetTalkUI(const yl::string & strName)
{
    return UILogicTalkUIToBaseTalkUI(strName, GetTalkUIFromCache(strName, false));
}

CUILogicBaseTalkUI * CTalkUIFactory::GetTalkUIFromCache(const yl::string & strName, bool bUnbind)
{
    //  CUILogicBaseTalkUI* pTalkUI = NULL;
    CDlgBaseTalkUI * pBaseTalkUI = NULL;
    if (PROCESSORNAME_DIAL == strName)
    {
        if (NULL == m_pDialingUI)
        {
            m_pDialingUI = new CDlgDialingUI();
        }

        pBaseTalkUI = m_pDialingUI;
    }
    else if (PROCESSORNAME_CONFMANAGER == strName)
    {
        if (NULL == m_pConfManagerUI)
        {
            m_pConfManagerUI = new CDlgConfManagerUI();
        }

        pBaseTalkUI = m_pConfManagerUI;
    }
    else if (PROCESSORNAME_ACDCALLINFO == strName)
    {
        if (NULL == m_pACDCallInfoUI)
        {
            m_pACDCallInfoUI = new CDlgACDCallInfoUI();
        }

        pBaseTalkUI = m_pACDCallInfoUI;
    }
    else if (PROCESSORNAME_RTPSTATUS == strName)
    {
        if (NULL == m_pRTPStatusUI)
        {
            m_pRTPStatusUI = new CDlgRTPStatusUI();
        }

        pBaseTalkUI = m_pRTPStatusUI;
    }
    else if (PROCESSORNAME_PARKNOTIFY == strName)
    {
        if (NULL == m_pParkNotifyUI)
        {
            m_pParkNotifyUI = new CDlgParkNotifyUI();
        }

        pBaseTalkUI = m_pParkNotifyUI;
    }
    else if (PROCESSORNAME_CONF == strName)
    {
        if (NULL == m_pConfUI)
        {
            m_pConfUI = new CDlgConfUI();
        }

        pBaseTalkUI = m_pConfUI;
    }
    else if (PROCESSORNAME_AUTOREDIAL == strName)
    {
        if (NULL == m_pAutoRedialUI)
        {
            m_pAutoRedialUI = new CDlgAutoRedialUI();
        }

        pBaseTalkUI = m_pAutoRedialUI;
    }
    else if (PROCESSORNAME_FINISH == strName)
    {
        if (NULL == m_pFinishUI)
        {
            m_pFinishUI = new CDlgFinishUI();
        }

        pBaseTalkUI = m_pFinishUI;
    }
    else if (PROCESSORNAME_RING == strName)
    {
        if (NULL == m_pRingUI)
        {
            m_pRingUI = new CDlgRingUI();
        }

        pBaseTalkUI = m_pRingUI;
    }
    else if (PROCESSORNAME_CONNECT == strName)
    {
        if (NULL == m_pConnectUI)
        {
            m_pConnectUI = new CDlgConnectUI();
        }

        pBaseTalkUI = m_pConnectUI;
    }
    else if (PROCESSORNAME_TALKING == strName)
    {
        if (NULL == m_pTalkingUI)
        {
            m_pTalkingUI = new CDlgTalkingUI();
        }

        pBaseTalkUI = m_pTalkingUI;
    }
    else if (PROCESSORNAME_NETCONF == strName)
    {
        if (NULL == m_pNetConfUI)
        {
            m_pNetConfUI = new CDlgNetConfUI();
        }

        pBaseTalkUI = m_pNetConfUI;
    }
    else if (PROCESSORNAME_NETCONFMGR == strName)
    {
        if (NULL == m_pNetConfMgrUI)
        {
            m_pNetConfMgrUI = new CDlgNetConfMgrUI();
        }

        pBaseTalkUI = m_pNetConfMgrUI;
    }

    if (NULL == pBaseTalkUI)
    {
        return NULL;
    }

    pBaseTalkUI->JudgeLoadLayout();

    CUILogicBaseTalkUI* pUILogicBase = BaseTalkUIToUILogicTalkUI(strName, pBaseTalkUI);
    if (NULL != pUILogicBase)
    {
        if (bUnbind)
        {
            ReleaseTalkUI(strName, pUILogicBase);
        }
        return pUILogicBase;
    }

    return NULL;
}

void CTalkUIFactory::ReleaseTalkUI(const yl::string & strProcessorName, CUILogicBaseTalkUI * pTalkUI)
{
    if (NULL != pTalkUI)
    {
        CBaseTalkUIProcessor * pProcessor = pTalkUI->GetBindUIProcessor();

        //设置绑定的UI 为空
        if (NULL != pProcessor)
        {
            pProcessor->SetBindUI(NULL);
        }

        pTalkUI->BindProcessor(NULL);
    }
}

void CTalkUIFactory::ReleaseAllUI()
{
    if (NULL != m_pACDCallInfoUI)
    {
        delete m_pACDCallInfoUI;
    }
    if (NULL != m_pAutoRedialUI)
    {
        delete m_pAutoRedialUI;
    }
    if (NULL != m_pConfUI)
    {
        delete m_pConfUI;
    }
    if (NULL != m_pConfManagerUI)
    {
        delete m_pConfManagerUI;
    }
    if (NULL != m_pConnectUI)
    {
        delete m_pConnectUI;
    }
    if (NULL != m_pFinishUI)
    {
        delete m_pFinishUI;
    }
    if (NULL != m_pParkNotifyUI)
    {
        delete m_pParkNotifyUI;
    }
    if (NULL != m_pRingUI)
    {
        delete m_pRingUI;
    }
    if (NULL != m_pRTPStatusUI)
    {
        delete m_pRTPStatusUI;
    }
    if (NULL != m_pTalkingUI)
    {
        delete m_pTalkingUI;
    }
    if (NULL != m_pNetConfUI)
    {
        delete m_pNetConfUI;
    }
    if (NULL != m_pNetConfMgrUI)
    {
        delete m_pNetConfMgrUI;
    }

    m_pACDCallInfoUI = NULL;
    m_pAutoRedialUI = NULL;
    m_pConfUI = NULL;
    m_pConfManagerUI = NULL;
    m_pConnectUI = NULL;
    m_pFinishUI = NULL;
    m_pParkNotifyUI = NULL;
    m_pRingUI = NULL;
    m_pRTPStatusUI = NULL;
    m_pTalkingUI = NULL;
    m_pNetConfUI = NULL;
    m_pNetConfMgrUI = NULL;
}

//因为多继承造成指针强转问题
//UILogic层的基础对话框转换成UI的基础对话框
CDlgBaseTalkUI * CTalkUIFactory::UILogicTalkUIToBaseTalkUI(yl::string strProcessorName, CUILogicBaseTalkUI * pUILogicBaseTalkUI)
{
    if (PROCESSORNAME_ACDCALLINFO == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgACDCallInfoUI *)pUILogicBaseTalkUI;
    }
    else if (PROCESSORNAME_AUTOREDIAL == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgAutoRedialUI *)pUILogicBaseTalkUI;
    }
    else if (PROCESSORNAME_CONF == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgConfUI *)pUILogicBaseTalkUI;
    }
    else if (PROCESSORNAME_CONFMANAGER == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgConfManagerUI *)pUILogicBaseTalkUI;
    }
    else if (PROCESSORNAME_CONNECT == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgConnectUI *)pUILogicBaseTalkUI;
    }
    else if (PROCESSORNAME_DIAL == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgDialingUI *)pUILogicBaseTalkUI;
    }
    else if (PROCESSORNAME_FINISH == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgFinishUI *)pUILogicBaseTalkUI;
    }
    else if (PROCESSORNAME_PARKNOTIFY == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgParkNotifyUI *)pUILogicBaseTalkUI;
    }
    else if (PROCESSORNAME_RING == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgRingUI *)pUILogicBaseTalkUI;
    }
    else if (PROCESSORNAME_RTPSTATUS == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgRTPStatusUI *)pUILogicBaseTalkUI;
    }
    else if (PROCESSORNAME_TALKING == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgTalkingUI *)pUILogicBaseTalkUI;
    }
    else if (PROCESSORNAME_NETCONF == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgNetConfUI *)pUILogicBaseTalkUI;
    }

    else if (PROCESSORNAME_NETCONFMGR == strProcessorName)
    {
        return (CDlgBaseTalkUI *)(CDlgNetConfMgrUI *)pUILogicBaseTalkUI;
    }


    //防止 使用异常
    return NULL;
}

//UI的基础对话框转换成UILogic层的基础对话框
CUILogicBaseTalkUI * CTalkUIFactory::BaseTalkUIToUILogicTalkUI(const yl::string strProcessorName, CDlgBaseTalkUI * pDlgBaseTalkUI)
{
    if (PROCESSORNAME_ACDCALLINFO == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgACDCallInfoUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_AUTOREDIAL == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgAutoRedialUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_CONF == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgConfUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_CONFMANAGER == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgConfManagerUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_CONNECT == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgConnectUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_DIAL == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgDialingUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_FINISH == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgFinishUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_PARKNOTIFY == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgParkNotifyUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_RING == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgRingUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_RTPSTATUS == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgRTPStatusUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_TALKING == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgTalkingUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_NETCONF == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgNetConfUI *)pDlgBaseTalkUI;
    }
    else if (PROCESSORNAME_NETCONFMGR == strProcessorName)
    {
        return (CUILogicBaseTalkUI *)(CDlgNetConfMgrUI *)pDlgBaseTalkUI;
    }

    //防止 使用异常
    return NULL;
}

}
