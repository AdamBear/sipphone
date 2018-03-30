#include "rtpstatusprocessor.h"
#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "talkuilogicframemanager.h"
#include "talkuilogicprocessorname.h"

#include "talk/talklogic/include/modtalklogic.h"
#include "talk/vqrtcp/include/modvqrtcp.h"
#include "feature/include/featurecommon.h"
#include "feature/include/modfeature.h"
#include "uilogicrtpstatusuibase.h"

LRESULT OnTalkUILogicRTPStatusMessage(msgObject & refMessage);

LRESULT OnTalkUILogicRTPStatusMessage(msgObject & refMessage)
{
    //获取当前使用的 talkuiprocessor
    CBaseTalkUIProcessor * pTalkUIProcessor = _TalkUILogicFrameManager.GetCurrentTalkUIProcessor();

    if (NULL == pTalkUIProcessor)
    {
        return 0;
    }

    //processor 名称是 RTPStatus  那么才需要交给该实例处理
    if (PROCESSORNAME_RTPSTATUS == pTalkUIProcessor->GetProcessorName())
    {
        CRTPStatusProcessor * pRTPStatusProcessor = (CRTPStatusProcessor *)pTalkUIProcessor;

        pRTPStatusProcessor->OnRTPStatusMessage(refMessage);

        return 1;
    }

    return 0;
}

CRTPStatusProcessor::CRTPStatusProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_RTPSTATUS)
    , m_bShowSwitch(false)
{
    m_pTalkUI = NULL;
}

CRTPStatusProcessor::~CRTPStatusProcessor()
{
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CRTPStatusProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    etl_RegisterMsgHandle(RTCP_UDPATE_WND, RTCP_UDPATE_WND, OnTalkUILogicRTPStatusMessage);

    //初始化设置 不显示 rtpswitch
    m_bShowSwitch = false;
    m_objRTPStatusUIData.Reset();
}

//释放前解除UI绑定前调用
void CRTPStatusProcessor::Uninit()
{
    etl_UnregisterMsgHandle(RTCP_UDPATE_WND, RTCP_UDPATE_WND, OnTalkUILogicRTPStatusMessage);

    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CRTPStatusProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicRTPStatusUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CRTPStatusProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//处理RTPStatus消息
void CRTPStatusProcessor::OnRTPStatusMessage(msgObject & refMessage)
{
    //更新数据
    if (0 == refMessage.wParam)
    {
        UpdateRTPStatusData();
    }
    else if (1 == refMessage.wParam) //退出显示
    {
        ExitRTPStatusUI();
    }
}

//设置数据
void CRTPStatusProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    //先更新一下 是否显示rtpstatusswitch
    m_bShowSwitch = vqrtcp_IsRTPStatusSwitch();

    //这边不调用 UpdateRTPStatusData 更新数据  在 SetTalkUIData 函数中调用更新
    //这个比较特殊
//  UpdateRTPStatusData();
}

// 通道键处理
bool CRTPStatusProcessor::OnChannelKeyPress(int iKey)
{
    if (SESSION_TALKING == m_eSessionState)
    {
        return OnTalkingChannelKeyPress(iKey);
    }
    else if (SESSION_LOCALCONF == m_eSessionState)
    {
        return OnConferenceChannelKeyPress(iKey);
    }

    return CBaseTalkUIProcessor::OnChannelKeyPress(iKey);
}

//talking状态通道键处理
bool CRTPStatusProcessor::OnTalkingChannelKeyPress(int iKey)
{
    //rtcp不处理
    ////为什么之前只判断 transfer 失败 ？？？？
    ////    if (m_bTransferFailed)
    //if (m_bTransferFailed || m_bCallParkFailed)
    //{
    //  //      ExitNotify();
    //  SendUIRequestAction(TALK_ACTION_EXIT_FAILD_INFO);
    //  // 通道按键继续往下传
    //}

    bool bHandled = false;
    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
            if (voice_IsCallCenterMode())
            {
                // callCenterMode交给基类处理
                bHandled = false;
            }
            else
            {
                // 通话中摘手柄时按免提键，则打开群听模式
                if (voice_GetHandsetStatus() && voice_GetHandfreeStatus())
                {
                    //这边肯定是处于 TS_TALK 状态的  因为是处于Hold、Held状态的话 就退出了
                    //if(m_eTalkState == TS_TALK)
                    //{
                    TALKUI_INFO("CTalkingUI : Open GroupListening");
                    //if (1 == configParser_GetConfigInt(kszGroupListenInTalkingEnable))
                    if (1 == configParser_GetConfigInt(kszGroupListenInTalkingEnable))
                    {
                        voice_SwitchChannel(CHANNEL_GROUP);
                    }
                    else
                    {
                        voice_SwitchChannel(CHANNEL_HANDFREE);
                    }
                    bHandled = true;
                    //}
                }
                else
                {
                    bHandled = false;
                }
            }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            if (!IsChannelKeyToExit(iKey))
            {
                bHandled = true;
                voice_SwitchChannel();
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }
    if (!bHandled)
    {
        bHandled = CBaseTalkUIProcessor::OnChannelKeyPress(iKey);
    }

    return bHandled;
}

//会议状态 通道键处理
bool CRTPStatusProcessor::OnConferenceChannelKeyPress(int iKey)
{
    bool bHandled = false;
    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
            if (voice_IsCallCenterMode())
            {
                // callCenterMode交给基类处理
                bHandled = false;
            }
            else
            {
                // 通话中摘手柄时按免提键，则打开群听模式
                if (voice_GetHandsetStatus() && voice_GetHandfreeStatus())
                {
                    //                  if (!m_bAllPartyOfConfHold)
                    //这边 本地肯定是不会处于Hold 状态的  因为如果处于Hold 状态 那么就退出rtp 界面了
                    //if(!m_bLocalHold)
                    //{
                    voice_SwitchChannel(CHANNEL_GROUP);
                    bHandled = true;
                    //}
                }
            }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            if (!IsChannelKeyToExit(iKey))
            {
                bHandled = true;
                voice_SwitchChannel();
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }

    if (!bHandled)
    {
        bHandled = CBaseTalkUIProcessor::OnChannelKeyPress(iKey);
    }

    return bHandled;
}

// 功能键处理
bool CRTPStatusProcessor::OnFunckeyPress(int iKey)
{
    switch (iKey)
    {
    case PHONE_KEY_CANCEL:
        {
            //这边就是刷新 通话 或者会议窗口
            ExitRTPStatusUI();

            return true;
        }
        break;
    case PHONE_KEY_OK:
        {
            SendUIRequestAction(TALK_ACTION_OK);
            return true;
        }
        break;

    case PHONE_KEY_LEFT:
    case PHONE_KEY_RIGHT:
        {
            if (m_bShowSwitch)
            {
                //切换显示 话路的 rtp 信息
                SwitchRTPStatus();

                return true;
            }

            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

// 数字键处理
//来电界面不需要处理数字键
bool CRTPStatusProcessor::OnDigitkeyPress(int iKey)
{
    return false;
}

//softKey 按键处理
bool CRTPStatusProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = false;

    switch (eSoftkey)
    {
    case ST_RTP_SWITCH:
        {
            SwitchRTPStatus();
            bHandled = true;
        }
        break;
    case ST_CANCEL:
        {
            ExitRTPStatusUI();

            bHandled = true;
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

//设置title
void CRTPStatusProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //rtp 不显示title, 这边暂时以 TRID_RTP_STATUS 保存，  t49 不显示这个title
    m_strTitle = _UILOGIC_LANG_TRAN(TRID_RTP_STATUS);
}

//设置图标列表
void CRTPStatusProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
{

}

//给子界面设置数据
void CRTPStatusProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objRTPStatusUIData, eReason);
    }

    //同时在这更新 ui 的rtpstatus 数据
    //一般的processor 是不在这 做其他处理的  只调用 settalkdata
    UpdateRTPStatusData();
}


//获取softkey数据信息
void CRTPStatusProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (m_bShowSwitch)
    {
        VecSoftkey.push_back(ST_RTP_SWITCH);
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }

    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_EMPTY);

    VecSoftkey.push_back(ST_CANCEL);
}

// 是否允许自定义softkey
bool CRTPStatusProcessor::IsAllowDefineSoftkey()
{
    return false;
}

// softkey处理(自定义类型)
void CRTPStatusProcessor::GetSoftkey(yl::string & strFile, yl::string & strStatus)
{

}

//退出RTPStatus显示
void CRTPStatusProcessor::ExitRTPStatusUI()
{
    talklogic_UpdateWnd();
}

//选择查看 RTP 话路信息
void CRTPStatusProcessor::SwitchRTPStatus()
{
    vqrtcp_RTPStatusSwitch();
}

//更新 RTPStatus 数据
void CRTPStatusProcessor::UpdateRTPStatusData()
{
    //更新切换 softkey 的状态
    m_bShowSwitch = vqrtcp_IsRTPStatusSwitch();

    SVQReportData objSVQReportData;

    //调用逻辑层接口获取rtpstatus 数据
    bool bResult = vqrtcp_RTPStatusGetCurrentReport(objSVQReportData);

    //如果读取rtp 数据不成功 那么 直接返回不需要设置数据了
    if (false == bResult)
    {
        return ;
    }

    //根据配置决定显示哪些选项
    const RTPConfigureList * pRTPlist = feature_RTPConfigGetEnableList();
    //获取到的 RTPList 配置项数据位空 那么返回
    if (NULL == pRTPlist)
    {
        return;
    }

    RTPConfigureList pList = *pRTPlist;

    RTPConfigureList::const_iterator iter = pList.begin();

    TALKUI_INFO("RTP Status Load data size[%d]", pList.size());

    //保存选择的 rtpstatus 数据列表
    talkui_vector<CRTPStatusData> vectorRTPStatusData;

    for (; iter != pList.end(); ++iter)
    {
        RTPConfigureItem item = *iter;
        if (!item.m_isEnable)
        {
            continue;
        }

        yl::string strTag = _UILOGIC_LANG_TRAN(item.m_strDisplayName.c_str()) + yl::string(": ");
        yl::string strValue = objSVQReportData.GetAttributeValueById(item.m_strIdName);

        CRTPStatusData objRTPStatusData;
        objRTPStatusData.strTag = strTag;
        objRTPStatusData.strValue = strValue;

        vectorRTPStatusData.push_back(objRTPStatusData);
    }

    //调用UI接口 更新rtpstatus 数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->UpdateUIRTPStatusData(vectorRTPStatusData);
    }
}
