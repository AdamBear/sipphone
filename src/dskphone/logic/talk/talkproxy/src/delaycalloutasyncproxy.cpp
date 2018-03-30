#include "delaycalloutasyncproxy.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/src/tipteladapter.h"
#include "voip/modvoip.h"


CallOutInfo::CallOutInfo(CCallInfo* p, LIST_INFO& list)
    :  CBaseTalkASyncProxy(p->GetSessionID()),
       m_list(list)
{
    /*相关变量 必须初始化，否则影响呼出行为*/
    m_eCallOutType = TCT_UNKNOWN;
    m_iMask = SIP_CALL_MASK_NORMAL;
    m_iBLAIndex = -1;
    m_iBLFIndex = -1;
    m_eBlaType = BLA_NONE;
    m_iEmergencyIndex = -1;
    m_strNumber.clear();
    m_strCode.clear();
    m_strInvokeID.clear();
    m_strExtNumber.clear();
#if IF_FEATURE_CTRIP_CTI
    m_strXUUI.clear();
#endif


    CallOut_Param& param = p->GetCallOutParam();
    int iAccountID = p->GetAccountID();

    // 允许呼出时设置类型
    if (param.eCallOutType != TCT_UNKNOWN)
    {
        p->m_eTalkCallOutType = param.eCallOutType;
        m_eCallOutType = param.eCallOutType;
    }

    if (param.bIntercom)
    {
        m_iMask |= SIP_CALL_MASK_INTERCOM;
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (!param.m_strCode.empty())
    {
        m_strCode = param.m_strCode;
    }
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    if (TCT_CALL_SILENT_MONITOR == p->m_eTalkCallOutType)
    {
        m_iMask |= SIP_CALL_MASK_BROADSOFT_SILENT_BARGIN;
    }
#endif //IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#if IF_FEATURE_CTRIP_CTI
    if (ctrip_IsControlByCtrip(iAccountID))
    {
        m_iMask |= SIP_CALL_MASK_UC_INFO;
    }
#endif //IF_FEATURE_CTRIP_CTI

#if IF_FEATURE_LOOPBACK
    if (configParser_GetConfigInt(kszLoopBackEnable) == 1)
    {
        TALK_INFO("Loop Back Callout");
        m_iMask |= SIP_CALL_MASK_LOOPBACK_CALL;
    }
#endif

    if (TCT_TIPTEL_AUTO_CALLOUT == p->m_eTalkCallOutType)
    {
        // 待SIP提供处理方式
        m_strInvokeID = TiptelA_GetInvokeID();
    }

    m_iAccountID = iAccountID;
    m_iCallID = p->GetCallID();

#if IF_FEATURE_CTRIP_CTI
    if (!param.strXUUI.empty())
    {
        // 网页发过来的local_number转给sip,添加到头域
        m_strXUUI = param.strXUUI;
    }
#endif

    if (param.iPickUpBLFIndex != -1)
    {
        m_iBLFIndex = param.iPickUpBLFIndex;          // 注意：PickUP BLF时，如果同时多路呼叫BLF号码，用户可以自己选择截答哪一路
        // 设置呼叫号码为param.strRemoteName;
        m_strNumber = param.strRemoteName;
    }
    else
    {
        TALK_INFO("m_bNeedToSeizeLine = %d, SeizeIndex = %d", p->m_bNeedToSeizeLine, m_iBLAIndex);

        if (acc_IsBLAEnable(iAccountID)
                && p->m_iBLAIndex >= 0)
        {
            m_eBlaType = (int)p->GetBLAType();
            m_iBLAIndex = p->m_iBLAIndex;
        }
    }

#if IF_BUG_27033 || IF_BUG_30965
    ChangeTo(m_strNumber, p->m_eDialDsskeyType);
#endif

    // 音频呼出:外部有指定用外部的，外部没指定默认是音频
    if (configParser_GetConfigInt(kszVideoTalkEnable) == TVO_OPTIONAL)
    {
        // 指针为空表示没有传入参数，采用默认音频
        if (param.bAudioOnly)
        {
            TALK_INFO("audio call");
            // 去掉视频标志位
            m_iMask &= ~SIP_CALL_MASK_VIDEO;
        }
    }

    m_list.push_back(this);

    // 延迟400ms呼出
    SetTimerOnce(400);
}

bool CallOutInfo::OnCallBack()
{
    // 内存泄露：无论对应的Routine存在与否, 均要清除缓存值
    CCallInfo* p = talklogic_GetCallInfoByCallId(m_iCallID);
    if (NULL != p)
    {
        TALK_INFO("CallOutInfo::OnCallBack CallOut Num[%s] CallID[%d] AccountID[%d]",
                  m_strNumber.c_str(), m_iCallID, m_iAccountID);

#ifdef IF_FEATURE_PSTN
        if (acc_IsPstnAccount(m_iAccountID))
        {
            if (!pstnadapter_Dial(acc_GetDeviceIDByAccountID(m_iAccountID), m_strNumber, m_iCallID))
            {
                TALK_INFO("Dial PSTN failed.");
                // 发送拨号失败消息，由ConnectRoutine统一处理
                bool bResult = false;
                etl_NotifyAppEx(false, PSTN_MSG_DIAL_RESULT, m_iAccountID, m_iCallID, sizeof(bool), &bResult);
            }
        }
        else
#endif // IF_FEATURE_PSTN
        {
            modVoip_MakeCall(*this);
        }
    }

    LIST_INFO::iterator it = app::find(m_list.begin(), m_list.end(), this);
    if (it != m_list.end())
    {
        m_list.erase(it);
    }

    // 删除自身
    delete this;

    return p != NULL;
}

#if IF_BUG_27033 || IF_BUG_30965
void CallOutInfo::ChangeTo(yl::string& strFullName, DIAL_DSSKEY_TYPE eDialDsskeyType)
{
    if (eDialDsskeyType == DDT_NONE)
    {
        return;
    }

    yl::string strTemp = "";
    switch (eDialDsskeyType)
    {
    case DDT_INTERCOM:
        {
            strTemp = "INTERCOM";
            break;
        }
#if IF_FEATURE_PAGING
    case DDT_PAGING:
        {
            strTemp = "PAGEGROUP";
            break;
        }
#endif
    case DDT_PICKUP:
        {
            strTemp = "PICKUPEXT";
            break;
        }
    case DDT_UNPARK:
        {
            strTemp = "UNPARK";
            break;
        }
    default:
        break;

    }

    yl::string::size_type nSipPos = strFullName.find("sip:");
    if (nSipPos != yl::string::npos)
    {
        strFullName.insert(nSipPos + 4, strTemp);
    }
    else
    {
        strFullName.insert(0, strTemp);
    }
}
#endif
