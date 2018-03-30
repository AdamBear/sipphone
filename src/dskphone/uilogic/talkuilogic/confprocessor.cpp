#include "confprocessor.h"
#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
//#include "settingui/include/modsettingui.h"
#include "talkuilogicframemanager.h"
#include "uilogicconfuibase.h"
#include "application/include/modapplication.h"

CConfProcessor::CConfProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_CONF)
{
    m_pTalkUI = NULL;

    m_bShowTalkTime = true;     //是否显示通话时间

#if IF_TALKUI_PSTN
    m_bHasSipLine = false;
#endif

    //是否已经添加过acd的softkey
    m_bACDSoftkeySet = false;
    //是否已经添加过rtp的softkey
    m_bRTPSoftkeySet = false;

    m_bCallingRoutineExist = false;
}

CConfProcessor::~CConfProcessor()
{
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CConfProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    m_objConfUIData.Reset();

    //与话路相关的图标
    m_vectorIconsWithCallID.clear();

#if IF_TALKUI_PSTN
    m_bHasSipLine = false;
#endif
}

//释放前解除UI绑定前调用
void CConfProcessor::Uninit()
{
    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CConfProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicConfUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CConfProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取 会议数据
CConfUIData * CConfProcessor::GetConfData()
{
    return &m_objConfUIData;
}

//获取hold状态
bool CConfProcessor::IsHold()
{
    return m_objConfUIData.bHoldConf;
}

//根据CallID 获取联系人信息
CContactInfo CConfProcessor::GetContactInfoByCallID(int iCallID)
{
    CContactInfo objContactInfo;

    YLList<CContactInfo>::iterator iteratorContactInfo;

    for (iteratorContactInfo = m_objConfUIData.listContactInfo.begin();
            m_objConfUIData.listContactInfo.end() != iteratorContactInfo; iteratorContactInfo++)
    {
        objContactInfo = *iteratorContactInfo;
        if (iCallID == objContactInfo.iCallID)
        {
            return objContactInfo;
        }

        //重置一下联系人信息
        objContactInfo.Reset();
    }

    return objContactInfo;

}

//根据callid 获取显示图标
void CConfProcessor::GetIconListByCallID(YLList<TALKUI_ICON> & listIcons, int iCallID)
{
    YLList<TALKUI_ICON>::iterator iteratorList = m_listIcons.begin();

    //先加入公共部分的图标
    for (; m_listIcons.end() != iteratorList; iteratorList++)
    {
        AddIconToList(listIcons, *iteratorList);
    }

    //再加入callID特有的图标

    int iIndex = -1;
    for (int i = 0; i < m_vectorIconsWithCallID.size(); i++)
    {
        if (iCallID == m_vectorIconsWithCallID[i].iCallID)
        {
            iIndex = i;
            break;

        }
    }

    //有找到对应的实例
    if (iIndex >= 0)
    {
        CCallIDWithIcons & objCallIDWithIcons = m_vectorIconsWithCallID[iIndex];

        for (int j = 0; j < objCallIDWithIcons.vectorIcons.size(); j++)
        {
            AddIconToList(listIcons, objCallIDWithIcons.vectorIcons[j]);
        }
    }
}

//获取当前话路的焦点联系人信息
bool CConfProcessor::GetFocusContactInfo(CContactInfo & objContactInfo)
{
    CUILogicConfUIBase * pTalkUIConf = (CUILogicConfUIBase *)GetBindUI();

    if (NULL != pTalkUIConf)
    {
        int iFocusCallID = pTalkUIConf->GetFocusCallID();

        YLList<CContactInfo>::iterator iteratorList = m_objConfUIData.listContactInfo.begin();
        if (iFocusCallID < 0
                && m_objConfUIData.listContactInfo.end() != iteratorList)
        {
            CContactInfo objContactInfoFind = *iteratorList;
            objContactInfo = objContactInfoFind;
            return true;
        }

        for (; m_objConfUIData.listContactInfo.end() != iteratorList; iteratorList++)
        {
            CContactInfo objContactInfoFind = *iteratorList;
            //callID相同直接返回
            if (objContactInfoFind.iCallID == iFocusCallID)
            {
                objContactInfo = objContactInfoFind;

                return true;
            }

        }
    }

    return false;

}

//判断是否多路通话同时允许显示hold
bool CConfProcessor::IsAllowShowHoldInTalk(YLList<CCallInfo *> & listCallInfo)
{
    //根据列表设置是否允许显示Hold信息
    bool bAllAllowHold = true;
    YLList<CCallInfo *>::iterator itCallInfo = listCallInfo.begin();
    for (int nIndex = 0; itCallInfo != listCallInfo.end()
            && nIndex < MAX_CONF_PARTY_NUM; ++itCallInfo, ++nIndex)
    {
        CCallInfo * pCallInfo = (CCallInfo *)(*itCallInfo);
        if (pCallInfo != NULL)
        {
            bAllAllowHold = true;

            if (!bAllAllowHold)
            {
                return bAllAllowHold;
            }
        }
    }

    return bAllAllowHold;
}

//设置数据
void CConfProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //根据逻辑层上传数据取数据
    DataLocalConf * pLocalConfData = (DataLocalConf *)(pData);

    if (NULL == pLocalConfData)
    {
        return;
    }

    yl::string strIncomingCallNum = "";

    //如果有获取到新来电号码 那么有来电的标识设置为true  否则 设置为false
    if (talklogic_GetIncomingCallNum(strIncomingCallNum))
    {
        if (pLocalConfData->bInConfManager)
        {
            SendUIRequestAction(TALK_ACTION_EXIT_CONF_MANAGER, 1);
        }

#if IF_BUG_29709
        int nDisplayMethod = configParser_GetConfigInt(kszDisplayMethod);
        if (nDisplayMethod == SHOW_NULL
                && "anonymous" != strIncomingCallNum.MakeLower()
                && "private number" != strIncomingCallNum.MakeLower())
        {
            m_objConfUIData.bHasCallComming = true;
            m_objConfUIData.strIncomingCallNum = " ";
        }
        else
#endif
        {
            m_objConfUIData.bHasCallComming = true;
            m_objConfUIData.strIncomingCallNum = strIncomingCallNum;
        }
    }
    else
    {
        m_objConfUIData.bHasCallComming = false;
        m_objConfUIData.strIncomingCallNum = "";
    }

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    CCallInfo * pTmpCallInfo = GetFocusCallInfo();
    if (pTmpCallInfo != NULL)
    {
        m_eRecordStatue = pTmpCallInfo->m_eRecordStatus;
    }

    m_objConfUIData.listCallInfo = pLocalConfData->listCallInfo;

    bool bShowFail = true;
    CALL_RECORD_ACTION_RESULT eFailResult = CALL_RECORD_NONE;
    YLList<CCallInfo *>::iterator iter = pLocalConfData->listCallInfo.begin();
    while (iter != pLocalConfData->listCallInfo.end() && *iter)
    {
        CALL_RECORD_ACTION_RESULT eResult = (*iter)->m_eRecordResult;
        if (eResult >= CALL_RECORD_START_FAILED
                && eResult <= CALL_RECORD_STOP_FAILED)
        {
            eFailResult = eResult;
        }
        else
        {
            bShowFail = false;
            break;
        }
        ++iter;
    }

    if (bShowFail)
    {
        if (NULL != m_pTalkUI)
        {
            m_pTalkUI->ShowErrorNote(eFailResult);
        }

        YLList<CCallInfo *>::iterator iter = pLocalConfData->listCallInfo.begin();
        while (iter != pLocalConfData->listCallInfo.end() && *iter)
        {
            CALL_RECORD_ACTION_RESULT eResult = (*iter)->m_eRecordResult;
            if (eResult >= CALL_RECORD_START_FAILED
                    && eResult <= CALL_RECORD_STOP_FAILED)
            {
                (*iter)->m_eRecordResult = CALL_RECORD_NONE;
            }
            ++iter;
        }
    }
#endif

//     //获取本地hold标志
//     m_objConfUIData.bLocalHold = GetCallInfoListHoldFlag(pLocalConfData->listCallInfo, false);

//     //获取本地是否所有路hold标志 http://10.2.1.199/Bug.php?BugID=89208
//     m_objConfUIData.bAllLocalHold = GetCallInfoListHoldFlag(pLocalConfData->listCallInfo);

    m_objConfUIData.bHoldConf = pLocalConfData->bHoldConf;

    //获取 held 标志
    m_objConfUIData.bRemoteHold = GetCallInfoListHeldFlag(pLocalConfData->listCallInfo, true);

    m_objConfUIData.bAllowSplit = pLocalConfData->bAllowSplit;

    m_objConfUIData.bAllowNewCall = pLocalConfData->bAllowNewCall;

#ifdef IF_TALKUI_FIVE_PARTY
    //是否允许显示 conference 的softkey
    m_objConfUIData.bAllowConf = pLocalConfData->bAllowConf;
#endif

    m_objConfUIData.bEncrypt = pLocalConfData->bEncrypt;

    //安全等级
    m_objConfUIData.bAllowSecurity = pLocalConfData->bAllowSecurity;
    if (IsShowSecurityClassification())
    {
        m_objConfUIData.strSecurityLevel = talklogic_GetTalkSecurityLevel(m_iSessionID);
    }
    else
    {
        m_objConfUIData.strSecurityLevel = "";
    }

    //http://10.2.1.199/Bug.php?BugID=84336 根据逻辑层标志决定是否显示hold
    m_objConfUIData.bAllowHold = IsAllowShowHoldInTalk(pLocalConfData->listCallInfo);

    //根据列表设置话路信息
    //先重置一下 FirstCallID
    m_objConfUIData.nFirstCallID = 0;

    bool bFirstRecord = false;
    //先把联系人列表中的数据清空
    m_objConfUIData.listContactInfo.clear();

    YLList<CCallInfo *>::iterator itCallInfo = pLocalConfData->listCallInfo.begin();
    CContactInfo objContactInfo;
    CCallInfo * pCallInfo = NULL;

#if IF_TALKUI_PSTN
    bool bHasSipLine = false;
#endif

    m_bCallingRoutineExist = false;

    //与话路相关的图标  先全部清除
    m_vectorIconsWithCallID.clear();

    bool bAllCodeSame = true;
    m_objConfUIData.eAudioCodec = ACT_NONE;
    AUDIO_CODEC_TYPE eConfAudioCodec = ACT_NONE;

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
                m_objConfUIData.nFirstCallID = pCallInfo->GetCallID();
                bFirstRecord = true;
            }

            //根据callinfo 数据获取 联系人信息
            GetContactInfoFromCallInfo(objContactInfo, pCallInfo);

            if (ROUTINE_CONNECTING == pCallInfo->m_eStateOfRoutine
                    || ROUTINE_RINGBACK == pCallInfo->m_eStateOfRoutine)
            {
                yl::string strCalling = _UILOGIC_LANG_TRAN(TRID_CALLING);
                objContactInfo.strName = strCalling + ":" + objContactInfo.strName;
                m_bCallingRoutineExist = true;
            }

            //插入到列表中
            m_objConfUIData.listContactInfo.push_back(objContactInfo);

#if IF_TALKUI_PSTN
            if (objContactInfo.m_eCallLineType == CALL_LINE_SIP)
            {
                bHasSipLine = true;
            }
            // 通话类型为PSTN，且账号不可用，说明对应的PSTN线路拔出
            else if (objContactInfo.m_eCallLineType == CALL_LINE_PSTN
                     && !acc_IsAccountIDValid(pCallInfo->GetAccountID()))
            {
                TALKUI_INFO("PSTN account #%d in conference is unavailable.", pCallInfo->GetAccountID());
                m_eNetworkStatus = TALK_NETWORK_STATUS_PSTN_FAIL;
            }
#endif

            AUDIO_CODEC_TYPE eCurrentAudioCodec = pCallInfo->GetAudioCodecType();
            if (ACT_NONE != eCurrentAudioCodec)
            {
                int iCallID = pCallInfo->GetCallID();
                if (eCurrentAudioCodec & ACT_HD)
                {
                    AddIconsByCallID(iCallID, TALKUI_ICON_HD);
                }
                else if (eCurrentAudioCodec & ACT_AMR)
                {
                    AddIconsByCallID(iCallID, TALKUI_ICON_AMR);
                }
                else if (eCurrentAudioCodec & ACT_OPUS)
                {
                    AddIconsByCallID(iCallID, TALKUI_ICON_OPUS);
                }
            }
            if (ACT_NONE == eConfAudioCodec)
            {
                eConfAudioCodec = eCurrentAudioCodec;
            }
            else
            {
                if (eConfAudioCodec != eCurrentAudioCodec)
                {
                    bAllCodeSame = false;
                }
            }
        }
    }

    //如果全部话路的高清类型都是一样的，则可以赋值作为会议的高清类型
    if (bAllCodeSame)
    {
        m_objConfUIData.eAudioCodec = eConfAudioCodec;
    }

#if IF_TALKUI_PSTN
    m_bHasSipLine = bHasSipLine;

    if (TALK_NETWORK_STATUS_PSTN_FAIL != m_eNetworkStatus
            && bHasSipLine)
    {
        if (FALSE == netIsNetworkReady())
        {
            m_eNetworkStatus = TALK_NETWORK_STATUS_FAIL;
        }
        else
        {
            m_eNetworkStatus = TALK_NETWORK_STATUS_UNKNOWN;
        }
    }
#endif
}

// 通道键处理
bool CConfProcessor::OnChannelKeyPress(int iKey)
{
    bool bHandled = false;

    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
#if IF_TALKUI_HANDFREE_DIAL
            if (acc_ExistAvailableAccount(ACCOUNT_TYPE_ALL))
            {
                // 新建一路
                SendUIRequestAction(TALK_ACTION_NEWCALL);
            }
            else
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
                        if (!m_objConfUIData.bHoldConf)
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
bool CConfProcessor::OnFunckeyPress(int iKey)
{
    switch (iKey)
    {
    // http://10.2.1.199/Bug.php?BugID=33941
    // 开启softkeyLayout的情况下按Hold键需要解Hold
    case PHONE_KEY_HOLD:
        {
            //http://10.2.1.199/Bug.php?BugID=88426 当bAllowHold为false的时候界面不显示hold/resume，所以硬按键要拦截
            if (!m_objConfUIData.bAllowHold)
            {
                return true;
            }

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
            //处理存在呼出话路的场景
            if (ProcessCallOutStatus())
            {
                return true;
            }

            //x键复用为 mute
            if (_TalkUILogicDelegate.IsMuteReplaceX())
            {
                SendUIRequestAction(TALK_ACTION_MUTE);

                return true;
            }

#if !IF_BUG_25055
            //本地hold的时候不允许退出
            if (m_objConfUIData.bHoldConf)
            {
                return true;
            }
#endif

            //这边肯定是conf界面  所以一定发送cancel
            SendUIRequestAction(TALK_ACTION_CANCEL);

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
bool CConfProcessor::OnDigitkeyPress(int iKey)
{
    //发送dtmf
    SendUIRequestAction(TALK_ACTION_DTMF, iKey);

    return true;
}

//softKey 按键处理
bool CConfProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
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
#if IF_TALKUI_FIVE_PARTY
    case ST_JOIN:
        {
            if (m_objConfUIData.bAllowJoin)
            {
                bHandled = SendUIRequestAction(TALK_ACTION_JOIN);
            }
            else
            {
                bHandled = true;
            }
        }
        break;
#endif
    case ST_REJECT:
        {
            // 有来电时的响应
            bHandled = SendUIRequestAction(TALK_ACTION_REJECT);
        }
        break;
    case ST_CANCEL:
        {
            //处理存在呼出话路的场景
            if (ProcessCallOutStatus())
            {
                //
            }
            else
            {
                SendUIRequestAction(TALK_ACTION_CANCEL);
            }

            bHandled = true;
        }
        break;
    case ST_CONF_MANAGE:
        {
            bHandled = true;
            SendUIRequestAction(TALK_ACTION_ENTER_CONF_MANAGER);
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
#ifndef _WIN32
#warning SPLIT_ALL
#endif
            //SendUIRequestAction(TALK_ACTION_SPLIT_ALL);
            SendUIRequestAction(TALK_ACTION_SPLIT);
            bHandled = true;
        }
        break;
    case ST_NEWCALL:
        {
            SendUIRequestAction(TALK_ACTION_NEWCALL);
            bHandled = true;
        }
        break;
    case ST_CONFERENCE:
        {
            SendUIRequestAction(TALK_ACTION_CONF);

            bHandled = true;
        }
        break;

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    case ST_ACD_DISPCODE:
        {
            if (CallCenter_IsDispCodeOn()
                    && m_objConfUIData.nFirstCallID > 0)
            {
                if (NULL != m_pTalkUI)
                {
                    m_pTalkUI->ShowACDDispCodeMenu(m_objConfUIData.nFirstCallID);
                }
            }

            bHandled = true;
        }
        break;
    case ST_ACD_EMERGENCY:
        {
            if (CallCenter_IsEmergencyEscalationOn()
                    && m_objConfUIData.nFirstCallID > 0)
            {
                if (NULL != m_pTalkUI)
                {
                    m_pTalkUI->ShowEmergencyMenu(m_objConfUIData.nFirstCallID);
                }
            }

            bHandled = true;
        }
        break;
    case ST_ACD_TRACE:
        {
            if (CallCenter_IsTraceOn()
                    && m_objConfUIData.nFirstCallID > 0)
            {
                CallCenter_OriginatedTrace(m_objConfUIData.nFirstCallID);
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
    case ST_SECURITY:
        {
            if (IsShowSecurityClassification())
            {
                int iLeastAccount = talklogic_GetAccountLeastLevFromLocalConf();
                if (m_pTalkUI != NULL)
                {
                    m_pTalkUI->ShowSecurityBox(iLeastAccount);
                }
            }

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
void CConfProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //根据逻辑层上传数据取数据
    DataLocalConf * pLocalConfData = (DataLocalConf *)(pData);

    //获取本地hold标志
    bool bHold = pLocalConfData->bHoldConf;

    //和lxh确认 如果是本地hold 要显示hold
    //held 和 mute 不需要特殊显示
    if (bHold)
    {
        m_strTitle = _UILOGIC_LANG_TRAN(TRID_HOLD);
    }
    else
    {
        m_strTitle = _UILOGIC_LANG_TRAN(TRID_CONFERENCE);
    }

#if IF_CONF_SECURITY_IN_TITLE
    yl::string strSecurityLevel = talklogic_GetTalkSecurityLevel(m_iSessionID);
    if (pLocalConfData->bAllowSecurity
            && !strSecurityLevel.empty())
    {
        if (bHold)
        {
            m_strTitle = _UILOGIC_LANG_TRAN(TRID_CONF);
        }

        m_strTitle += " - ";
        m_strTitle += _UILOGIC_LANG_TRAN(strSecurityLevel.c_str());
    }
#endif
}

//设置图标列表
void CConfProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
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

    //获取是否有录音图标, 只要有一路录音 那么就是录音的
    YLList<CCallInfo *>::iterator itr = pLocalConfData->listCallInfo.begin();
    CCallInfo * pCallInfoFirst = NULL;
    if (itr != pLocalConfData->listCallInfo.end())
    {
        pCallInfoFirst = (*itr);
    }

    if (NULL != pCallInfoFirst)
    {
        ////不是UNKNOWN类型的 要添加录音图标
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

    if (m_objConfUIData.eAudioCodec & ACT_HD)
    {
        AddTalkUIIcon(TALKUI_ICON_HD, true);
    }
    else if (m_objConfUIData.eAudioCodec & ACT_AMR)
    {
        AddTalkUIIcon(TALKUI_ICON_AMR, true);
    }
    else if (m_objConfUIData.eAudioCodec & ACT_OPUS)
    {
        AddTalkUIIcon(TALKUI_ICON_OPUS, true);
    }
}

//给子界面设置数据
void CConfProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objConfUIData, eReason);
    }
}

void CConfProcessor::DealSecurityResult(const yl::string & strSecurityLevel)
{
    int iAccount = talklogic_GetAccountLeastLevFromLocalConf();
    //设置当前账号的安全等级
    talklogic_ModifyAccountOverrideLevel(iAccount, strSecurityLevel);
}

bool CConfProcessor::IsAllowDefineSoftkey()
{
#if IF_TALKUI_PSTN
    CALL_LINE_TYPE eCallLineType;
    if (!m_bHasSipLine      //会议中不包含sip话路
            || ((talklogic_GetIncomingCallLineType(eCallLineType)
                 && CALL_LINE_PSTN == eCallLineType))) // 来电为PSTN
    {
        return false;
    }
#endif

    return true;
}

bool CConfProcessor::AllowSoftkeyShow(SoftKey_TYPE eType)
{
    if (ST_SECURITY == eType)
    {
        return IsShowSecurityClassification();
    }
    else if (ST_RTP_STATUS == eType)
    {
        return IsAllowRTPSoftkey();
    }

    return CBaseTalkUIProcessor::AllowSoftkeyShow(eType);
}

bool CConfProcessor::IsShowSecurityClassification()
{
    return m_objConfUIData.bAllowSecurity;
}

bool CConfProcessor::IsAllowRTPSoftkey()
{
    if (!m_bRTPSoftkeySet
            && !m_bCallingRoutineExist
            && !m_objConfUIData.bHoldConf
            && !m_objConfUIData.bRemoteHold
            && 1 == configParser_GetConfigInt(kszDisplayVQReportOnUI))
    {
        return true;
    }

    return false;
}

void CConfProcessor::GetAlwaysBeforeSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    m_bACDSoftkeySet = false;
    m_bRTPSoftkeySet = false;
}

void CConfProcessor::RelayoutSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    for (int i = 0; i < VecSoftkey.size();)
    {
        if (ST_ACD_EMERGENCY == VecSoftkey[i])
        {
            m_bACDSoftkeySet = true;
        }
        else if (ST_RTP_STATUS == VecSoftkey[i])
        {
            m_bRTPSoftkeySet = true;
        }
        else if (ST_HOLD == VecSoftkey[i])
        {
            if (IsShowResumeSoftkey())
            {
                VecSoftkey[i] = ST_FAR_RESUME;
            }
        }

        ++i;
    }
}

void CConfProcessor::GetAlwaysAfterSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (m_bUseCustomSoftkey
            && !_TalkUILogicDelegate.IsCustomDynamicSoftkeyOn())
    {
#if IF_TALKUI_USB_ENABLE
        AddUSBRecordSoftkey(VecSoftkey);
#endif
        return;
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    SetACDSoftkey(VecSoftkey);
#endif

#if IF_TALKUI_USB_ENABLE
    //添加USB截图相关softkey
    AddUSBScreenshotSoftkey(VecSoftkey);

    //添加USB录音相关softkey
    if (!AddUSBRecordSoftkey(VecSoftkey))
#endif
    {
        //没有本地录制softkey的情况下，才添加bsft录制softkey
        SetRecordSoftkey(VecSoftkey);
    }

    if (IsShowSecurityClassification()) //在endcall之后插入security softkey
    {
        VecSoftkey.push_back(ST_SECURITY);
    }
}

//获取softkey数据信息
void CConfProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (m_objConfUIData.bHasCallComming)
    {
        //有来电 显示来电的softkey
        if (_TalkUILogicDelegate.IsSoftkeyChangedCallIncome())
        {
            GetRingStatusSoftKeyBarData(VecSoftkey);
        }
        //不允许改变softkey时，显示通话的softkey
        else
        {
            GetTalkStatusSoftKeyBarData(VecSoftkey);
        }
    }
    //没有来电显示 通话的softkey
    else
    {
        GetTalkStatusSoftKeyBarData(VecSoftkey);
    }
}

// softkey处理(自定义类型)
void CConfProcessor::GetSoftkey(yl::string & strFile, yl::string & strStatus)
{
    strFile = "Talking.xml";

#if IF_FEATURE_CUSTOM_SOFTKEY
    if (m_objConfUIData.bHasCallComming
            && _TalkUILogicDelegate.IsSoftkeyChangedCallIncome())
    {
        strStatus = "ConferencedNewCallIn";
    }
    else if (m_objConfUIData.bHoldConf)
    {
        strStatus = "ConferencedHold";
    }
    else
    {
        strStatus = "Conferenced";
    }
#else
    strStatus = "Conferenced";
#endif
}

//获取通话状态下的softkey
void CConfProcessor::GetTalkStatusSoftKeyBarData(YLVector<SoftKey_TYPE> & VecSoftkey)
{
#if IF_TALKUI_FIVE_PARTY
//  if (m_objConfUIData.bHoldConf)
//  {
//      // 需求：如果处于hold状态则不显示conf manager
//      VecSoftkey.push_back(ST_EMPTY);
//  }
//  else
    {
        // 支持五方会议的机型，第一个软键为Manage
        VecSoftkey.push_back(ST_CONF_MANAGE);
    }
#else
    //http://10.2.1.199/Bug.php?BugID=102589
    if (m_objConfUIData.bAllowSplit)
    {
        VecSoftkey.push_back(ST_SPLIT);
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }
#endif

#ifdef IF_FEATURE_ENTRANCE_GUARD
    if (application_IsDoorLineCall(GetSessionID()))
    {
        VecSoftkey.push_back(ST_OPENDOOR);
    }
#endif

    //要允许hold 才需要显示这些softkey http://10.2.1.199/Bug.php?BugID=84336
    if (m_objConfUIData.bAllowHold)
    {
        //处于hold状态 需要显示resume 的softkey  非hold状态要显示hold的softkey
        //http://10.2.1.199/Bug.php?BugID=89208 当所有路都hold才显示resume
        if (m_objConfUIData.bHoldConf)
        {
            VecSoftkey.push_back(ST_RESUME);
        }
        else
        {
            VecSoftkey.push_back(ST_HOLD);
        }
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    SetACDSoftkey(VecSoftkey);
#endif

#if IF_TALKUI_FIVE_PARTY
    // 支持五方会议的机型，第三个软键为Conf
    if (acc_ExistAvailableAccount(ACCOUNT_TYPE_ALL)/* 存在可用账号才显示NewCall */)
    {
        if (m_objConfUIData.bHoldConf)
        {
            VecSoftkey.push_back(ST_NEWCALL);
        }
        else
        {
            if (m_objConfUIData.bAllowConf)
            {
                VecSoftkey.push_back(ST_CONFERENCE);
            }
            else
            {
                VecSoftkey.push_back(ST_NEWCALL);
            }
        }
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }
#else
    //如果判断conf需要显示manager的softkey才显示
    if (_TalkUILogicDelegate.IsConfShowManagerSoftkey())
    {
        VecSoftkey.push_back(ST_CONF_MANAGE);
    }
#endif

    if (m_bCallingRoutineExist)
    {
        VecSoftkey.push_back(ST_CANCEL);
    }
    else
    {
        // 设置二级菜单界面对应SoftKeybar的数据
        VecSoftkey.push_back(ST_ENDCALL);
    }

    //conference只要还有一路在通话中就要显示rtp status
    if (IsAllowRTPSoftkey())
    {
        m_bRTPSoftkeySet = true;
        VecSoftkey.push_back(ST_RTP_STATUS);
    }
}

//获取来电状态下的softkey
void CConfProcessor::GetRingStatusSoftKeyBarData(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    //暂时认为 这个时候 最需要处理的是来电  所以不需要显示split的softkey了  黑白屏是没有显示的
    // http://10.3.5.199/Bug.php?BugID=69931  最终根据这个bug 修改显示softkey
    //  VecSoftkey.push_back(ST_EMPTY);

    // 设置二级菜单界面对应2nd SoftKeybar的数据
    //http://10.2.1.199/Bug.php?BugID=103479 SPLIT应该排在answer前面
#ifdef IF_FEATURE_ENTRANCE_GUARD
    if (application_IsDoorLineCall())
    {
        VecSoftkey.push_back(ST_OPENDOOR);
    }
#endif

    //cp机型不需要这个空softkey
#if !IF_TALKUI_FIVE_PARTY
    VecSoftkey.push_back(ST_EMPTY);
#endif

#if IF_TALKUI_FIVE_PARTY
    if (m_objConfUIData.bAllowJoin)
    {
        VecSoftkey.push_back(ST_JOIN);
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }
#endif

    VecSoftkey.push_back(ST_ANSWER);

    bool bAllowReject = true;
#if IF_TALKUI_PSTN
    CALL_LINE_TYPE eCallLineType;
    if (talklogic_GetIncomingCallLineType(eCallLineType) && eCallLineType == CALL_LINE_PSTN)
    {
        // PSTN来电不支持Reject
        bAllowReject = false;
    }
#endif
#ifdef IF_FEATURE_EMERGENCY
    if (talklogic_IsEmergencyRecall())
    {
        //紧急呼叫不能拒接
        bAllowReject = false;
    }
#endif

#if IF_BUG_38956
    if (0 == configParser_GetConfigInt(kszRejectIncomingCallSwitch))
    {
        bAllowReject = false;
    }
#endif

    if (bAllowReject)
    {
        VecSoftkey.push_back(ST_REJECT);
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }

    //来电界面不显示rtp
    if (m_bCallingRoutineExist)
    {
        VecSoftkey.push_back(ST_CANCEL);
    }
    else
    {
        // 设置二级菜单界面对应SoftKeybar的数据
        VecSoftkey.push_back(ST_ENDCALL);
    }
}

// 添加Broadsoft ACD功能的Softkey
void CConfProcessor::SetACDSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    //如果已经设置过，则不再设置
    if (m_bACDSoftkeySet)
    {
        return;
    }

    m_bACDSoftkeySet = true;

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (CallCenter_IsACDOn()
            && ACD_BROADSOFT == Get_ACDType()
            && configParser_GetConfigInt(kszBroadsoftActive) != 0
            && ACD_GetAcdStatus() != AS_IDLE)
    {
        //检测是否是ACD关联的账号，不是的话不显示
        bool bShowSoftKey = false;
        for (YLList<CContactInfo>::const_iterator it = m_objConfUIData.listContactInfo.begin();
                it != m_objConfUIData.listContactInfo.end(); it++)
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

        // if(CallCenter_IsTraceOn())
        // {
        //  VecSoftkey.push_back(ST_ACD_TRACE);
        // }

        //http://10.3.5.199/Bug.php?BugID=62477
        if (CallCenter_IsEmergencyEscalationOn())
        {
            VecSoftkey.push_back(ST_ACD_EMERGENCY);
        }
    }
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
}

//添加与callID相关的图标
void CConfProcessor::AddIconsByCallID(int iCallID, TALKUI_ICON eTalkUIIcon)
{
    int iIndex = -1;

    CCallIDWithIcons objCallIDWithIcons;

    for (int i = 0; i < m_vectorIconsWithCallID.size(); i++)
    {
        objCallIDWithIcons = m_vectorIconsWithCallID[i];

        if (iCallID == objCallIDWithIcons.iCallID)
        {
            iIndex = i;
            break;
        }
    }

    //说明有找到 , 直接加入到实例中就可以了
    if (iIndex >= 0)
    {
        m_vectorIconsWithCallID[iIndex].AddIcon(eTalkUIIcon);
    }
    else        //说明没有找到 那么要往队列中插入一个数据
    {
        CCallIDWithIcons objCallIDWithIconsAdd;
        objCallIDWithIconsAdd.iCallID = iCallID;
        objCallIDWithIconsAdd.AddIcon(eTalkUIIcon);

        //加入到队列中
        m_vectorIconsWithCallID.push_back(objCallIDWithIconsAdd);
    }
}

//往队列中添加图标
void CConfProcessor::AddIconToList(YLList<TALKUI_ICON> & listIcons, TALKUI_ICON eTalkUIIcon)
{
    bool bFindResult = false;

    YLList<TALKUI_ICON>::iterator iteratorList = listIcons.begin();

    for (; listIcons.end() != iteratorList; iteratorList++)
    {
        if (*iteratorList == eTalkUIIcon)
        {
            bFindResult = true;
            break;
        }
    }

    //没有找到 才需要插入
    if (!bFindResult)
    {
        listIcons.push_back(eTalkUIIcon);
    }
}

//是否显示resume的softkey
bool CConfProcessor::IsShowResumeSoftkey()
{
    if (m_objConfUIData.bAllowHold)
    {
        if (m_objConfUIData.bHoldConf)
        {
            return true;
        }
    }

    return false;
}

//是否显示Hold的softeky
bool CConfProcessor::IsShowHoldSoftkey()
{
    if (m_objConfUIData.bAllowHold)
    {
        if (!m_objConfUIData.bHoldConf)
        {
            return true;
        }
    }

    return false;
}

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
bool CConfProcessor::IfNeedCallRecording()
{
    return !m_objConfUIData.bHoldConf;
}
#endif


bool CConfProcessor::ProcessCallOutStatus()
{
    int iCallingCallID = 0;
    YLList<CCallInfo*>::iterator itCallInfo = m_objConfUIData.listCallInfo.begin();
    for (; itCallInfo != m_objConfUIData.listCallInfo.end(); ++itCallInfo)
    {
        CCallInfo* pCallInfo = (CCallInfo*)(*itCallInfo);

        if (NULL != pCallInfo
                && (pCallInfo->m_eStateOfRoutine == ROUTINE_RINGBACK
                    || pCallInfo->m_eStateOfRoutine == ROUTINE_CONNECTING))
        {
            iCallingCallID = pCallInfo->GetCallID();
        }
    }

    if (iCallingCallID != 0)
    {
        SendUIRequestAction(TALK_ACTION_REMOVE_CONF_PARTY, iCallingCallID);
        return true;
    }

    return false;
}
