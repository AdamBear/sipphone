#include "modtalklistener.h"
#include "listencenter.h"
#include "basetalklistener.h"
#include "talklistener_inc.h"

/* UI业务 */
#include "talkadapter/src/talkadapterlistener.h"
#include "talkadapter/src/talkupdatelistener.h"


size_t CScopedSigBlock::m_uDepth = 0;

void modtklsn_init()
{
    AUTO_SCOPED_TIMER;

    ListenCenter;

    /* 注意初始化顺序，对回调顺序的影响，sig接收函数顺序和ETL回调顺序 */

    new CBackLightListener;
    new CCommonListener;
    new CBWListener;
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    new CBroadsoftRecordListener;
#endif
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    new CUSBRecordListener;
#endif
#ifdef IF_FEATURE_RTCPXR
    new CRTCPListener;
#endif
    new CEventListener;
    new CDialPlanListener;
    new CDsskeyBindListener;
    new CMuteListener;
    new CCallCenterListener;
    new CFWDDNDListener;
    new CContractListener;
    new CCallMiscListener;
#ifdef IF_FEATURE_ENTRANCE_GUARD
    new CDooorPhoneListener;
#endif
    new CCommonRecordListener;
    new CAutoAnswerListener;

    new CSipMsgFliter;
    /* SCA挂断刷新要求在SipMsg处理之后 */
    new CSCAListener;
    new CVoiceListener;

    /* UI业务默认最后执行，其它业务需插在之前完成 */
    new CTalkAdapterListener;
    /* 最后一项才是刷新 */
    new CTalkUpdateListener;

    ListenCenter.Sort();
}

#if IF_FEATURE_EXTEND_LOG

int modtklsn_emit_sig(TALK_SIGNAL_TYPE sig, const SignalParams& sigp /*= SignalParams()*/, int flag /*= SIG_METHOD_DEF*/, LPCSTR func /*= NULL*/, size_t line /*= 0*/)
{
    if (CScopedSigBlock::m_uDepth > 0)
    {
        TALK_DBG("block sig[%d][%s] flag[%d] call[%d][%d] sigp[%d][%d][%d][%s] pos[%s:%d]", sig, GetSignalName(sig), flag,
                 sigp.iSessionID, sigp.iCallID, sigp.wParam.iValue, sigp.lParam.iValue, sigp.tParam.iValue, sigp.strData.c_str(), func, line);

        return false;
    }

    TALK_DBG("emit sig[%d][%s] flag[%d] call[%d][%d] sigp[%d][%d][%d][%s] pos[%s:%d]", sig, GetSignalName(sig), flag,
             sigp.iSessionID, sigp.iCallID, sigp.wParam.iValue, sigp.lParam.iValue, sigp.tParam.iValue, sigp.strData.c_str(), func, line);

    sigp.eSignal = sig;
    return ListenCenter.Emit(flag, sig, sigp);
}

#else

int modtklsn_emit_sig(TALK_SIGNAL_TYPE sig, const SignalParams& sigp /*= SignalParams()*/, int flag /*= TALK_SIG_DEF*/)
{
    if (CScopedSigBlock::m_uDepth > 0)
    {
        TALK_DBG("block sig[%d][%s] flag[%d] call[%d][%d] sigp[%d][%d][%d][%s]", sig, GetSignalName(sig), flag,
                 sigp.iSessionID, sigp.iCallID, sigp.wParam.iValue, sigp.lParam.iValue, sigp.tParam.iValue, sigp.strData.c_str());

        return false;
    }

    sigp.eSignal = sig;
    return ListenCenter.Emit(flag, sig, sigp);
}
#endif

CListenInfo* modtklsn_get_privdata(int iSessionID)
{
    return ListenCenter.GetData(iSessionID);
}

bool modtklsn_get_mute_status()
{
    return CMuteListener::m_bGlobalMute;
}

