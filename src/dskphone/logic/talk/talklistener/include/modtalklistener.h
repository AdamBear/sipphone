#pragma once

#include "talk/talklistener/include/talksignaliddefine.h"
#include "talk/talklistener/include/talklistenertypes.h"
#include "talk/talkproxy/include/helpclass.h"

class CScopedSigBlock
    : public AUTO_REF_COUNT, protected app::StackObjectBase
{
public:
    CScopedSigBlock()
        : AUTO_REF_COUNT(m_uDepth)
    {
    }

    static size_t   m_uDepth;
};

#define AUTO_SCOPED_SIG_BLOCK  CScopedSigBlock _unused_scoped_sig_block_guard

void modtklsn_init();

#if IF_FEATURE_EXTEND_LOG
/* 默认同时发同步和异步两种信号 */
int modtklsn_emit_sig(TALK_SIGNAL_TYPE sig, const SignalParams& sigp = SignalParams(), int flag = SIG_METHOD_DEF, LPCSTR func = NULL, size_t line = 0);

#define modtklsn_emit(sig, sigp)        modtklsn_emit_sig(sig, sigp, SIG_METHOD_DEF, __FUNCTION__, __LINE__)

#define modtklsn_emit_ex(sig)           modtklsn_emit_sig(sig, SignalParams(), SIG_METHOD_DEF, __FUNCTION__, __LINE__)

/* 只发一种的接口 */

/* 只作为同步信号发送 */
#define modtklsn_emit_sync(sig, sigp)   modtklsn_emit_sig(sig, sigp, SIG_METHOD_SEND, __FUNCTION__, __LINE__)

/* 只作为异步信号发送 */
#define modtklsn_emit_async(sig, sigp)  modtklsn_emit_sig(sig, sigp, SIG_METHOD_POST, __FUNCTION__, __LINE__)

/* 作为fliter发送 */
#define modtkflt_emit(sig, sigp)        (modtklsn_emit_sig(sig, sigp, SIG_METHOD_REQUEST, __FUNCTION__, __LINE__) & SIG_RET_BREAK)

#else

/* 默认同时发同步和异步两种信号 */
int modtklsn_emit_sig(TALK_SIGNAL_TYPE sig, const SignalParams& sigp = SignalParams(), int flag = SIG_METHOD_DEF);

#define modtklsn_emit(sig, sigp)        modtklsn_emit_sig(sig, sigp)

#define modtklsn_emit_ex(sig)           modtklsn_emit(sig, SignalParams())

/* 只发一种的接口 */

/* 只作为同步信号发送 */
#define modtklsn_emit_sync(sig, sigp)   modtklsn_emit_sig(sig, sigp, SIG_METHOD_SEND)

/* 只作为异步信号发送 */
#define modtklsn_emit_async(sig, sigp)  modtklsn_emit_sig(sig, sigp, SIG_METHOD_POST)

/* 作为fliter发送 */
#define modtkflt_emit(sig, sigp)        (modtklsn_emit_sig(sig, sigp, SIG_METHOD_REQUEST) & SIG_RET_BREAK)
#endif

CListenInfo* modtklsn_get_privdata(int iSessionID);

bool modtklsn_get_mute_status();
