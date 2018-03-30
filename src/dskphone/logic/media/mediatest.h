/************************************************************************/
/*
    Media Test 1.0
    1、添加mediatest 为 CMediaManager的友元
    2、开启测试宏IS_MEDIA_TEST
    3、编译、话机运行
    4、话机启动定时5s跑完测试用例
*/
/************************************************************************/
#ifndef _MEDIA_TEST_H_
#define _MEDIA_TEST_H_
#include "commondefine.h"

#ifdef IS_MEDIA_TEST
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/callinfo.h"

#define _mediaTest (CMediaTest::GetInstance())

class CMediaTest
{
public:
    CMediaTest()
    {
        m_bVideoMute = false;
        m_bHaveVideoHold = false;
        m_bHaveVideoRing = false;
        m_bIsLocalConf = false;
        m_bIsVideoTalking = false;
    }

    ~CMediaTest()
    {
    }

public:
    static CMediaTest & GetInstance();

    bool OnTimer(unsigned int Timer);

    void init();

    void Run();

    CCallInfo * GetCallInfo(int iCallId);

    bool test_StartAudio(int iCallId);
    bool test_StopAudio(int iCallId);

    bool test_StartVideo(int iCallId);

    bool test_VideoParam(bool bReset);

    bool test_HoldVideo(bool bHold, int iCallId);
    bool test_StopVideo(int iCallId);
    bool test_SetVideoLayout(bool bClear, int iNumber);
    bool test_CtrlLocalVideo(bool bOpen);
    bool test_MuteVideo(bool bMute, int iCallId);
    bool test_MultiVideo(int iNumber);
    bool test_MultiStopVideo(int iNumber);
    bool test_KeyframeSync(int iCallId);

    bool GetVideoSize();

public:
    YLList<CCallInfo> testlistCallInfo;
    int iTimerID;
    bool m_bVideoMute;
    bool m_bHaveVideoHold;
    bool m_bHaveVideoRing;
    bool m_bIsLocalConf;
    bool m_bIsVideoTalking;
};
#endif //#ifdef IS_MEDIA_TEST
#endif // _MOD_MEDIA_H_
