#include "confmanagerprocessor.h"
#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
//#include "settingui/include/modsettingui.h"
#include "uilogicconfmanageruibase.h"

#include "talkuilogicframemanager.h"
#include "talkuiprocessorlist.h"
#include "application/include/modapplication.h"

CConfManagerProcessor::CConfManagerProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_CONFMANAGER)
    , m_bAllowNewCall(true)
{
    m_pTalkUI = NULL;
}

CConfManagerProcessor::~CConfManagerProcessor()
{
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CConfManagerProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    m_objConfManagerUIData.Reset();

    m_bAllowNewCall = true;
}

//释放前解除UI绑定前调用
void CConfManagerProcessor::Uninit()
{
    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CConfManagerProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicConfManagerUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CConfManagerProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取 会议数据
CConfManagerUIData * CConfManagerProcessor::GetConfManagerUIData()
{
    return &m_objConfManagerUIData;
}

//设置数据
void CConfManagerProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    yl::string strIncomingCallNum = "";

    //如果有获取到新来电号码 那么有来电的标识设置为true  否则 设置为false
    if (talklogic_GetIncomingCallNum(strIncomingCallNum))
    {
        return;

        m_objConfManagerUIData.bHasCallComming = true;
        m_objConfManagerUIData.strIncomingCallNum = strIncomingCallNum;
    }
    else
    {
        m_objConfManagerUIData.bHasCallComming = false;
        m_objConfManagerUIData.strIncomingCallNum = "";
    }

    //根据逻辑层上传数据取数据
    DataLocalConf * pLocalConfData = (DataLocalConf *)(pData);

    m_bAllowNewCall = pLocalConfData->bAllowNewCall;

    //获取本地hold标志
    m_objConfManagerUIData.bLocalHold = pLocalConfData->bHoldConf;

    //获取 held 标志
    m_objConfManagerUIData.bRemoteHold = GetCallInfoListHeldFlag(pLocalConfData->listCallInfo, true);

    //根据列表设置话路信息
    //先重置一下 FirstCallID
    m_objConfManagerUIData.nFirstCallID = 0;

    bool bFirstRecord = false;

    //先把联系人列表中的数据清空
    m_objConfManagerUIData.listContactInfo.clear();

    YLList<CCallInfo *>::iterator itCallInfo = pLocalConfData->listCallInfo.begin();
    CContactInfo objContactInfo;
    CCallInfo * pCallInfo = NULL;

    //循环读取联系人信息
    for (; pLocalConfData->listCallInfo.end() != itCallInfo; itCallInfo++)
    {
        pCallInfo = *itCallInfo;

        if (NULL != pCallInfo)
        {
            if (!bFirstRecord
#if IF_TALKUI_LINE_DEVICE
                    && ROUTINE_LINE_DEVICE != pCallInfo->m_eStateOfRoutine
#endif
               )
            {
                m_objConfManagerUIData.nFirstCallID = pCallInfo->GetCallID();
                bFirstRecord = true;
            }

            //根据callinfo 数据获取 联系人信息
            GetContactInfoFromCallInfo(objContactInfo, pCallInfo);
            //插入到列表中
            m_objConfManagerUIData.listContactInfo.push_back(objContactInfo);
        }
    }
}

// 通道键处理
bool CConfManagerProcessor::OnChannelKeyPress(int iKey)
{
    bool bHandled = false;

    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
#if IF_TALKUI_ONHOOK_EXIT_CONFMANAGER
            bHandled = true;

            bool bAllNewCall = talklogic_BeAllowNewCall();
            bool bExistDial = talklogic_IsDialSessionExist();
            if (bAllNewCall)
            {
                SendUIRequestAction(TALK_ACTION_EXIT_CONF_MANAGER);
            }

            if (bAllNewCall && !bExistDial && acc_ExistAvailableAccount(ACCOUNT_TYPE_ALL))
            {
                SendUIRequestAction(TALK_ACTION_NEWCALL);
            }

            break;
#endif

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
                    if (!m_objConfManagerUIData.bLocalHold)
                    {
                        voice_SwitchChannel(CHANNEL_GROUP);
                        bHandled = true;
                    }
                }
            }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
#if IF_TALKUI_ONHOOK_EXIT_CONFMANAGER
            //CP 挂手柄, 只退出confmanager
            bHandled = true;
            SendUIRequestAction(TALK_ACTION_EXIT_CONF_MANAGER);

            break;
#endif

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
bool CConfManagerProcessor::OnFunckeyPress(int iKey)
{
    switch (iKey)
    {
    // http://10.2.1.199/Bug.php?BugID=33941
    // 开启softkeyLayout的情况下按Hold键需要解Hold
    case PHONE_KEY_HOLD:
        {
            //Hold or Resume
            SendUIRequestAction(TALK_ACTION_HOLD_SWITCH);

            return true;
        }
        break;
    case PHONE_KEY_MUTE:
        {
            //hwz 确认  hold状态下 不能mute 这个在逻辑层会有限制
            //Mute按键
            SendUIRequestAction(TALK_ACTION_MUTE);

            return true;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            //这边是退出 confmanager 界面
            SendUIRequestAction(TALK_ACTION_EXIT_CONF_MANAGER);

            return true;
        }
        break;
    case PHONE_KEY_OK:
        {
            SendUIRequestAction(TALK_ACTION_OK);
            return true;
        }
        break;

    //case PHONE_KEY_LEFT:          //这个交由界面处理
    //case PHONE_KEY_RIGHT:
    //case PHONE_KEY_UP:
    //case PHONE_KEY_DOWN:
    //  {

    //  }
    //  break;
    default:
        break;
    }

    return false;
}

// 数字键处理
bool CConfManagerProcessor::OnDigitkeyPress(int iKey)
{
    //发送dtmf
//     SendUIRequestAction(TALK_ACTION_DTMF, iKey);

    return true;
}

//softKey 按键处理
bool CConfManagerProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = false;

    switch (eSoftkey)
    {
    case ST_HOLD:
    case ST_RESUME:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_HOLD_SWITCH);
        }
        break;
    case ST_ANSWER:
        {
            // 有来电时的响应
            bHandled = SendUIRequestAction(TALK_ACTION_ANSWER);
        }
        break;
    case ST_REJECT:
        {
            // 有来电时的响应
            bHandled = SendUIRequestAction(TALK_ACTION_REJECT);
        }
        break;
    case ST_CANCEL:
        {
            SendUIRequestAction(TALK_ACTION_EXIT_CONF_MANAGER);
            bHandled = true;
        }
        break;
    case ST_BACK:       //退出confmanager
        {
            bHandled = true;
            SendUIRequestAction(TALK_ACTION_EXIT_CONF_MANAGER);
        }
        break;
    case ST_ENDCALL:    //结束通话
        {
            bHandled = true;
            SendUIRequestAction(TALK_ACTION_CANCEL);
        }
        break;
    //case ST_BACK:     //在conf界面不会有 back 的softkey
    //  {
    //      if (m_bInManage)
    //      {
    //          bHandled = TRUE;
    //          SendUIRequestAction(TALK_ACTION_EXIT_CONF_MANAGER);
    //      }
    //  }
    //  break;
    case ST_CONF_REMOVE:
        {
            if (NULL != m_pTalkUI)
            {
                SendUIRequestAction(TALK_ACTION_REMOVE_CONF_PARTY, m_pTalkUI->GetFocusCallID());
            }
            bHandled = true;
        }
        break;
    case ST_CONF_FAR_MUTE:
        {
            if (NULL != m_pTalkUI)
            {
                SendUIRequestAction(TALK_ACTION_CONF_FAR_MUTE, m_pTalkUI->GetFocusCallID(), 1);
            }
            bHandled = true;
        }
        break;
    case ST_CONF_UNMUTE:
        {
            if (NULL != m_pTalkUI)
            {
                SendUIRequestAction(TALK_ACTION_CONF_FAR_MUTE, m_pTalkUI->GetFocusCallID(), 0);
            }
            bHandled = true;
        }
        break;
    case ST_SPLIT:
        {
            SendUIRequestAction(TALK_ACTION_SPLIT, m_pTalkUI->GetFocusCallID());
            bHandled = true;
        }
        break;
#ifndef _WIN32
#warning
#endif
    /*
        case ST_SPLIT_ALL:
            {
                SendUIRequestAction(TALK_ACTION_SPLIT_ALL);
                bHandled = true;
            }
            break;
    */
    case ST_FAR_HOLD:
        {
            SendUIRequestAction(TALK_ACTION_CONF_FAR_HOLD, m_pTalkUI->GetFocusCallID(), 1);
            bHandled = true;
        }
        break;
    case ST_FAR_RESUME:
        {
            SendUIRequestAction(TALK_ACTION_CONF_FAR_HOLD, m_pTalkUI->GetFocusCallID(), 0);
            bHandled = true;
        }
        break;

    case ST_NEWCALL:
        {
            SendUIRequestAction(TALK_ACTION_NEWCALL);
            bHandled = true;
        }
        break;

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    case ST_ACD_DISPCODE:
        {
            if (CallCenter_IsDispCodeOn()
                    && m_objConfManagerUIData.nFirstCallID > 0)
            {
                if (NULL != m_pTalkUI)
                {
                    m_pTalkUI->ShowACDDispCodeMenu(m_objConfManagerUIData.nFirstCallID);
                }
            }

            bHandled = true;
        }
        break;
    case ST_ACD_EMERGENCY:
        {
            if (CallCenter_IsEmergencyEscalationOn()
                    && m_objConfManagerUIData.nFirstCallID > 0)
            {
                if (NULL != m_pTalkUI)
                {
                    m_pTalkUI->ShowEmergencyMenu(m_objConfManagerUIData.nFirstCallID);
                }
            }

            bHandled = true;
        }
        break;
    case ST_ACD_TRACE:
        {
            if (CallCenter_IsTraceOn()
                    && m_objConfManagerUIData.nFirstCallID > 0)
            {
                CallCenter_OriginatedTrace(m_objConfManagerUIData.nFirstCallID);
            }

            bHandled = true;
        }
        break;
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
    case ST_RTP_STATUS:
        {
            yl::string strUIName = PROCESSORNAME_RTPSTATUS;
            _TalkUILogicFrameManager.UpdateUIByProcessorName(strUIName);

            bHandled = true;
        }
        break;
    default:
        {
            bHandled = false;
        }
        break;
    }

    return bHandled;
}

//设置title
void CConfManagerProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    m_strTitle = _UILOGIC_LANG_TRAN(TRID_CONFERENCE_MANAGER);
}

//设置图标列表
void CConfManagerProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //先把数据重置一下
    m_listIcons.clear();

    //根据逻辑层上传数据取数据
    DataLocalConf * pLocalConfData = (DataLocalConf *)(pData);
    //添加语音加密图标
    if (true == pLocalConfData->bEncrypt)
    {
        AddTalkUIIcon(TALKUI_ICON_VOICEENCRY, true);
    }

    //获取是否有录音图标
    YLList<CCallInfo *>::iterator itr = pLocalConfData->listCallInfo.begin();
    CCallInfo * pCallInfoFirst = (*itr);
    if (NULL != pCallInfoFirst)
    {
        ICON_TYPE eIconType = GetRecordIconTypeFromCallInfo(pCallInfoFirst);
        TALKUI_ICON eTalkUIIcon = GetTalkUIIconByIconType(eIconType);

        //不是UNKNOWN类型的 要添加录音图标
        if (TALKUI_ICON_NONE != eTalkUIIcon)
        {
            AddTalkUIIcon(eTalkUIIcon, true);
        }

        ////不是UNKNOWN类型的 要添加录音图标
        //if(IT_UNKNOWN != GetRecordIconTypeFromCallInfo(pCallInfoFirst))
        //{
        //  AddTalkUIIcon(TALKUI_ICON_RECORD, true);
        //}
    }

    //必须要有通话参与方
    if (0 < pLocalConfData->listCallInfo.size())
    {
        //高清标志
        bool bHD = true;

        itr = pLocalConfData->listCallInfo.begin();
        for (; pLocalConfData->listCallInfo.end() != itr; itr++)
        {
            CCallInfo * pCallInfo = (CCallInfo *)(*itr);
            if (NULL != pCallInfo)
            {
                bHD = bHD && pCallInfo->IsHDTalking();
            }
        }

        //高清通话
        if (true == bHD)
        {
            AddTalkUIIcon(TALKUI_ICON_HD, true);
        }
    }
}

//给子界面设置数据
void CConfManagerProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objConfManagerUIData, eReason);
    }
}


//获取softkey数据信息
void CConfManagerProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    //没有来电显示 通话的softkey
    if (false == m_objConfManagerUIData.bHasCallComming)
    {
        GetTalkStatusSoftKeyBarData(VecSoftkey);
    }
    else    //有来电 显示来电的softkey
    {
        GetRingStatusSoftKeyBarData(VecSoftkey);
    }
}

// softkey处理(自定义类型)
void CConfManagerProcessor::GetSoftkey(yl::string & strFile, yl::string & strStatus)
{
    strFile = "Talking.xml";
    strStatus = "ConferenceManager";
}

//重新整理softkey，如果包含far mute或far hold则需要根据状态来变化softkey
void CConfManagerProcessor::RelayoutSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    CContactInfo objContactInfo;
    //获取焦点联系人信息
    bool bResult = GetFocusContactInfo(objContactInfo);
    if (!bResult)
    {
        return;
    }

    for (int i = 0; i < VecSoftkey.size(); ++i)
    {
#ifndef _WIN32
#warning
#endif
        /*
        if (ST_CONF_FAR_MUTE == VecSoftkey[i])
        {
            if (objContactInfo.bFarMute)
            {
                VecSoftkey[i] = ST_CONF_UNMUTE;
            }
        }
        else
        */

        if (ST_FAR_HOLD == VecSoftkey[i])
        {
            if (objContactInfo.bLocalHold)
            {
                VecSoftkey[i] = ST_FAR_RESUME;
            }
        }
    }
}

// 是否允许自定义softkey
bool CConfManagerProcessor::IsAllowDefineSoftkey()
{
#if IF_TALKUI_FIVE_PARTY
    CContactInfo objContactInfo;
    //获取焦点联系人信息
    bool bResult = GetFocusContactInfo(objContactInfo);

    if (bResult && objContactInfo.m_bReceived)
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    return false;
#endif
}

//获取通话状态下的softkey
void CConfManagerProcessor::GetTalkStatusSoftKeyBarData(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    CContactInfo objContactInfo;

#ifdef IF_FEATURE_ENTRANCE_GUARD
    if (application_IsDoorLineCall(GetSessionID()))
    {
        VecSoftkey.push_back(ST_OPENDOOR);
    }
#endif

    //获取焦点联系人信息
    bool bResult = GetFocusContactInfo(objContactInfo);

    //如果还是未接起的状态
    if (bResult && !objContactInfo.m_bReceived)
    {
        VecSoftkey.push_back(ST_EMPTY);
    }
    //否则判断far mute状态
    else if (bResult && (true == objContactInfo.bFarMute))
    {
        VecSoftkey.push_back(ST_CONF_UNMUTE);
    }
    else
    {
        VecSoftkey.push_back(ST_CONF_FAR_MUTE);
    }

    //linedevice无法remove
    if (CALL_LINE_DEVICE != objContactInfo.m_eCallLineType)
    {
        VecSoftkey.push_back(ST_CONF_REMOVE);
    }

#if IF_TALKUI_FIVE_PARTY
    // 支持五方会议的机型特制：Manage菜单的第三个按键显示为Split
    VecSoftkey.push_back(ST_SPLIT_ALL);
#else
    //是否要判断一下最大通话数？？？
    //之前版本的 逻辑层数据中是没有  这个标志位的  需要逻辑层先加上  同时要判断加上空格
    if (m_bAllowNewCall)
    {
        VecSoftkey.push_back(ST_NEWCALL);
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }
#endif

    //根据v81 把 back 放在 这个位置
    VecSoftkey.push_back(ST_BACK);

    if ((bResult && objContactInfo.m_bReceived)
            && !m_objConfManagerUIData.bLocalHold
            && !m_objConfManagerUIData.bRemoteHold
            && 1 == configParser_GetConfigInt(kszDisplayVQReportOnUI))
    {
        VecSoftkey.push_back(ST_RTP_STATUS);
    }
}

//获取来电状态下的softkey
void CConfManagerProcessor::GetRingStatusSoftKeyBarData(YLVector<SoftKey_TYPE> & VecSoftkey)
{
#ifdef IF_FEATURE_ENTRANCE_GUARD
    if (application_IsDoorLineCall())
    {
        VecSoftkey.push_back(ST_OPENDOOR);
    }
    else
#endif
        //暂时认为 这个时候 最需要处理的是来电  所以不需要显示split的softkey了  黑白屏是没有显示的
        VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_ANSWER);
    VecSoftkey.push_back(ST_REJECT);

    //根据v81 把 back 放在 这个位置
    VecSoftkey.push_back(ST_BACK);

    //conference只要还有一路在通话中就要显示rtp status
    if (!m_objConfManagerUIData.bLocalHold
            && !m_objConfManagerUIData.bRemoteHold
            && 1 == configParser_GetConfigInt(kszDisplayVQReportOnUI))
    {
        VecSoftkey.push_back(ST_RTP_STATUS);
    }
}

// 添加Broadsoft ACD功能的Softkey
void CConfManagerProcessor::SetACDSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (CallCenter_IsACDOn()
            && ACD_BROADSOFT == Get_ACDType()
            && configParser_GetConfigInt(kszBroadsoftActive) != 0
            && ACD_GetAcdStatus() != AS_IDLE)
    {
        //检测是否是ACD关联的账号，不是的话不显示
        bool bShowSoftKey = false;
        talkui_vector<CContactInfo>::iterator it = m_objConfManagerUIData.listContactInfo.begin();
        for (;
                it != m_objConfManagerUIData.listContactInfo.end()
                ; it++)
        {
            CContactInfo objContactInfo = *it;
            /*(CCallInfo*)(it->dataEntry)*/;
            if (ACD_GetAccountId() == objContactInfo.iSipAccount)
            {
                bShowSoftKey = true;
                break;
            }
        }
        if (!bShowSoftKey)
        {
            return;
        }

        //http://10.3.5.199/Bug.php?BugID=38061
        //http://10.2.1.199/Bug.php?BugID=36509
        //http://10.2.1.199/Bug.php?BugID=33934
        //在conf 界面 去掉
        //if(CallCenter_IsDispCodeOn())
        //{
        //  VecSoftkey.push_back(ST_ACD_DISPCODE);
        //}

        //if(CallCenter_IsEmergencyEscalationOn())
        //{
        //  VecSoftkey.push_back(ST_ACD_EMERGENCY);
        //}
    }
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
}

//根据CallID 获取联系人信息
bool CConfManagerProcessor::GetContactInfoByCallID(int iCallID, CContactInfo & objContactInfo)
{
    talkui_vector<CContactInfo>::iterator iteContactInfo;

    //循环查找
    for (iteContactInfo = m_objConfManagerUIData.listContactInfo.begin() ;
            m_objConfManagerUIData.listContactInfo.end() != iteContactInfo ; iteContactInfo++)
    {
        CContactInfo objContactInfoFind = *iteContactInfo;

        //callID相同 那么就返回true
        if (iCallID == objContactInfoFind.iCallID)
        {
            objContactInfo = objContactInfoFind;
            return true;
        }
    }

    return false;

}

//获取焦点联系人信息
bool CConfManagerProcessor::GetFocusContactInfo(CContactInfo & objContactInfo)
{
    if (NULL != m_pTalkUI)
    {
        return GetContactInfoByCallID(m_pTalkUI->GetFocusCallID(), objContactInfo);
    }

    return false;
}

