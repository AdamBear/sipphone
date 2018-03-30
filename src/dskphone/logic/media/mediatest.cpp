#ifdef IS_MEDIA_TEST
#include "commondefine.h"
#include "mediatest.h"
#include "modmedia.h"
#include "service_ipvp.h"
#include "cmediamanager.h"

// 视频通话id
#define CALL_1  35002
#define CALL_2  35004
#define CALL_3  35006
#define CALL_4  35008
#define CALL_5  35010

// 音频通话id
#define CALL_11 35052
#define CALL_12 35054
#define CALL_13 35056
#define CALL_14 35058
#define CALL_15 35060

CMediaTest & CMediaTest::GetInstance()
{
    static CMediaTest s_Instance;
    return s_Instance;
}

bool CMediaTest::OnTimer(unsigned int Timer)
{
    if (Timer == (unsigned int)&iTimerID)
    {
        timerKillThreadTimer((UINT)&iTimerID);
        Run();
    }
    return true;
}

// 处理Lync相关消息
static LRESULT OnMediaMessage(msgObject & msg)
{
    switch (msg.message)
    {
    case TM_TIMER:
        {
            CMediaTest::GetInstance().OnTimer(msg.wParam);
        }
        break;
    default:
        break;
    }

    return 0;
}

void CMediaTest::init()
{
    VOICE_INFO("## Test media init");
    SingleMsgReg(TM_TIMER, OnMediaMessage);
    timerSetThreadTimer((UINT)&iTimerID, 5000);
}

void CMediaTest::Run()
{
    int iCase = 1;
    VOICE_INFO("####Media test begin");
    // 音频start
    VOICE_INFO("####Test case %d: start audio and check VideoManager not to do anything", iCase++);
    test_StartAudio(CALL_11);
    if (GetVideoSize() == 0)
    {
        VOICE_INFO("####Test case success\n\n");
    }

    // 音频stop
    VOICE_INFO("####Test case %d: stop audio and check VideoManager not to do anything", iCase++);
    test_StopAudio(CALL_11);

    // 音频stop
    VOICE_INFO("####Test case %d: stop audio and check VideoManager not to do anything", iCase++);
    test_StopAudio(CALL_12);

    // 视频start，stop
    VOICE_INFO("####Test case %d: start video and check VideoManager to do", iCase++);
    test_StartVideo(CALL_1);
    test_StopVideo(CALL_1);

    // 音频hold unhold
    VOICE_INFO("####Test case %d: hold audio and check VideoManager not to do anything", iCase++);
    test_StartAudio(CALL_11);
    test_HoldVideo(true, CALL_11);
    test_HoldVideo(false, CALL_11);
    test_StopAudio(CALL_11);

    // 视频hold/unhold
    VOICE_INFO("####Test case %d: hold video and check VideoManager to do", iCase++);
    test_StartVideo(CALL_1);
    test_HoldVideo(true, CALL_1);

    // 灯
    test_HoldVideo(false, CALL_1);
    test_StopVideo(CALL_1);


    // 会议视频hold/unhold
    VOICE_INFO("####Test case %d: Conf hold video and check VideoManager to do", iCase++);
    test_StartVideo(CALL_1);
    test_StartVideo(CALL_2);
    m_bIsLocalConf = true;
    test_HoldVideo(true, CALL_1);

    // 灯
    test_HoldVideo(false, CALL_1);
    test_StopVideo(CALL_1);
    test_StopVideo(CALL_2);

    // 视频mute/unmute
    VOICE_INFO("####Test case %d: mute video and check VideoManager to do", iCase++);
    test_StartVideo(CALL_1);
    m_bVideoMute = true;
    test_MuteVideo(true, CALL_1);

    m_bVideoMute = false;
    test_MuteVideo(false, CALL_1);
    test_StopVideo(CALL_1);

    // 布局
    VOICE_INFO("####Test case %d: set video layout and check VideoManager to do", iCase++);
    for (int i = 0; i < 6; i++)
    {
        VOICE_INFO("####Test case: set video layout layout[%d]", i);
        test_SetVideoLayout(false, i);
    }

    VOICE_INFO("####Test case %d: clear video layout and check VideoManager to do", iCase++);
    test_SetVideoLayout(true, 0);

    // 关键帧
    VOICE_INFO("####Test case %d: keyframe sync and check VideoManager to do", iCase++);
    test_StartVideo(CALL_1);
    test_KeyframeSync(CALL_1);
    test_StopVideo(CALL_1);

    // 本地视频
    VOICE_INFO("####Test case %d: local video and check VideoManager to do", iCase++);
    test_CtrlLocalVideo(true);
    test_CtrlLocalVideo(false);

    // 多路视频
    VOICE_INFO("####Test case %d: multi video and check VideoManager to do", iCase++);
    for (int i = 0; i < 6; i++)
    {
        VOICE_INFO("####Test case %d: start video num[%d]", iCase, i);
        test_MultiVideo(i);

        VOICE_INFO("####Test case %d: stop video num[%d]", iCase, i);
        test_MultiStopVideo(i);
    }

    // 视频参数
    VOICE_INFO("####Test case %d: start video and check Video param", iCase++);
    test_VideoParam(false);

    // 视频参数
    VOICE_INFO("####Test case %d: start video and check Video param", iCase++);
    test_VideoParam(true);

    VOICE_INFO("####Media test end!!!!!");
}

CCallInfo * CMediaTest::GetCallInfo(int iCallId)
{
    VOICE_INFO("Get Call info [%d]", iCallId);
    YLList<CCallInfo>::iterator it = testlistCallInfo.begin();
    for (; it != testlistCallInfo.end(); ++it)
    {
        CCallInfo & info = *it;
        if (info.m_iCallId == iCallId)
        {
            return &info;
        }
    }
    return NULL;
}

bool CMediaTest::test_StartAudio(int iCallId)
{
    CCallInfo info;
    info.m_iCallId = iCallId;
    info.m_tVPMParam.mode |= IPVP_FL_VAM_AUDIO;
    testlistCallInfo.push_back(info);

    etl_NotifyApp(true, TALK_MSG_TALK_START, iCallId, 0);
    return true;
}

bool CMediaTest::test_StopAudio(int iCallId)
{
    etl_NotifyApp(true, TALK_MSG_TALK_STOP, iCallId, 0);

    VOICE_INFO("Stop audio");
    YLList<CCallInfo>::iterator it = testlistCallInfo.begin();
    for (; it != testlistCallInfo.end(); ++it)
    {
        VOICE_INFO("Stop audio [%d]", testlistCallInfo.size());

        CCallInfo & info = (*it);
        if (info.m_iCallId == iCallId)
        {
            testlistCallInfo.erase(it);
            return true;
        }
    }
    VOICE_INFO("Stop audio 2");
    return true;
}

bool CMediaTest::test_VideoParam(bool bReset)
{
    // 1、未初始化参数
    CCallInfo info;

    info.m_iCallId = CALL_1;
    info.m_tVPMParam.mode |= IPVP_FL_VAM_VIDEO;
    testlistCallInfo.push_back(info);

    etl_NotifyApp(true, TALK_MSG_TALK_START, info.m_iCallId, 0);

    // 正常参数
    info.m_iCallId = CALL_2;
    info.m_tVPMParam.mode |= IPVP_FL_VAM_VIDEO;
    if (bReset)
    {
        strcpy(info.m_tVPMParam.video.enc_list[0].codec_name, "H264");
        strcpy(info.m_tVPMParam.video.dec_list[0].codec_name, "H264");
        info.m_stVideoExt.m_stLocal.m_iLevel = 41;
        info.m_stVideoExt.m_stLocal.m_iMaxMBPS = 245760;
        info.m_stVideoExt.m_stRemote.m_iMaxFS = 8192;
    }
    else
    {
        strcpy(info.m_tVPMParam.video.enc_list[0].codec_name, "H263");
        strcpy(info.m_tVPMParam.video.dec_list[0].codec_name, "H263");
        info.m_stVideoExt.m_stLocal.m_iCIF4 = 1;
        info.m_stVideoExt.m_stLocal.m_iCIF = 1;
        info.m_stVideoExt.m_stRemote.m_iCIF4 = 1;
        info.m_stVideoExt.m_stRemote.m_iCIF4 = 1;
    }
    testlistCallInfo.push_back(info);

    etl_NotifyApp(true, TALK_MSG_TALK_START, info.m_iCallId, 0);

    // 2、空参数
    info.m_iCallId = CALL_3;
    info.m_tVPMParam.mode |= IPVP_FL_VAM_VIDEO;
    memset(info.m_tVPMParam.video.enc_list[0].codec_name, 0, IPVP_MAX_CODECNAME);
    memset(info.m_tVPMParam.video.dec_list[0].codec_name, 0, IPVP_MAX_CODECNAME);
    testlistCallInfo.push_back(info);

    etl_NotifyApp(true, TALK_MSG_TALK_START, info.m_iCallId, 0);


    // 不支持参数
    info.m_iCallId = CALL_4;
    info.m_tVPMParam.mode |= IPVP_FL_VAM_VIDEO;
    strcpy(info.m_tVPMParam.video.enc_list[0].codec_name, "Yealink");
    strcpy(info.m_tVPMParam.video.dec_list[0].codec_name, "Yealink");
    testlistCallInfo.push_back(info);

    etl_NotifyApp(true, TALK_MSG_TALK_START, info.m_iCallId, 0);


    // 超长参数
    info.m_iCallId = CALL_5;
    info.m_tVPMParam.mode |= IPVP_FL_VAM_VIDEO;
    strncpy(info.m_tVPMParam.video.enc_list[0].codec_name, "H264abcdeabcdeabcdeabcdeabcdeabcde", 16);
    strncpy(info.m_tVPMParam.video.dec_list[0].codec_name, "H264bbbbbbbbbbcccccccccddddddddddd", 16);
    testlistCallInfo.push_back(info);

    etl_NotifyApp(true, TALK_MSG_TALK_START, info.m_iCallId, 0);
    return true;
}

bool CMediaTest::test_StartVideo(int iCallId)
{
    CCallInfo info;
    info.m_iCallId = iCallId;
    info.m_tVPMParam.mode |= IPVP_FL_VAM_VIDEO;
    testlistCallInfo.push_back(info);

    etl_NotifyApp(true, TALK_MSG_TALK_START, iCallId, 0);
    return true;
}

bool CMediaTest::test_HoldVideo(bool bHold, int iCallId)
{
    YLList<CCallInfo>::iterator it = testlistCallInfo.begin();
    for (; it != testlistCallInfo.end(); ++it)
    {
        CCallInfo & info = *it;
        if (info.m_iCallId == iCallId)
        {
            info.m_bLocalHold = bHold;
            VOICE_ERR("hold find this");
            break;
        }
    }
    etl_NotifyApp(true, TALK_MSG_TALK_HOLD, iCallId, 0);
    return true;
}

bool CMediaTest::test_MuteVideo(bool bMute, int iCallId)
{
    etl_NotifyApp(true, TALK_MSG_VIDEO_MUTE, iCallId, 0);
    return true;
}

bool CMediaTest::test_StopVideo(int iCallId)
{
    etl_NotifyApp(true, TALK_MSG_TALK_STOP, iCallId, 0);

    YLList<CCallInfo>::iterator it = testlistCallInfo.begin();
    for (; it != testlistCallInfo.end(); ++it)
    {
        CCallInfo & info = *it;
        if (info.m_iCallId == iCallId)
        {
            testlistCallInfo.erase(it);
            return true;
        }
    }
    return true;
}

bool CMediaTest::test_MultiStopVideo(int iNumber)
{
    int iOldNum = ipvp_get_speeches(mkGetHandle());

    if (iNumber
            && test_StopVideo(CALL_1))
    {
        iNumber--;
        int iNewNum = GetVideoSize();

        if (iOldNum - 1 != iNewNum)
        {
            VOICE_ERR("Test 1 stopVideo fail");
        }
    }

    if (iNumber
            && test_StopVideo(CALL_2))
    {
        iNumber--;
        int iNewNum = GetVideoSize();

        if (iOldNum - 2 != iNewNum)
        {
            VOICE_ERR("Test 2 stopVideo fail");
        }
    }

    if (iNumber
            && test_StopVideo(CALL_3))
    {
        iNumber--;
        int iNewNum = GetVideoSize();

        if (iOldNum - 3 != iNewNum)
        {
            VOICE_ERR("Test 3  stopVideo fail");
        }
    }

    if (iNumber
            && test_StopVideo(CALL_4))
    {
        iNumber--;
        int iNewNum = GetVideoSize();

        if (iOldNum - 4 != iNewNum)
        {
            VOICE_ERR("Test 4  stopVideo fail");
        }
    }

    if (iNumber
            && test_StopVideo(CALL_5))
    {
        int iNewNum = GetVideoSize();

        if (iOldNum - 5 != iNewNum)
        {
            VOICE_ERR("Test 5 stopVideo fail");
        }
    }
}

bool CMediaTest::test_SetVideoLayout(bool bClear, int iNumber)
{
    DisplayMonitor display;
    if (bClear)
    {
        modmedia_SetVideoLayout(display);
        return true;
    }

    chRect win;
    win.SetRect(0, 0, 1920, 1080);
    // 极大值
    display.AddCameraLayout(win);
    int iHdmi = iNumber;
    if (iNumber-- > 0)
    {
        win.SetRect(0, 0, 1280, 720);
        display.AddVideoLayout(CALL_1, win);
    }

    if (iNumber-- > 0)
    {
        win.SetRect(0, 0, 0, 0);
        // 极小值
        display.AddVideoLayout(CALL_2, win);
    }

    if (iNumber-- > 0)
    {
        win.SetRect(-1, -2, 2000, 110);
        // 非法值
        display.AddVideoLayout(CALL_3, win);
    }
    if (iNumber-- > 0)
    {
        win.SetRect(1, 1, 1919, 1079);

        display.AddVideoLayout(CALL_4, win);
    }

    if (iNumber-- > 0)
    {
        win.SetRect(1, 1, 400, 240);

        display.AddVideoLayout(CALL_5, win);
    }

    // 极大值
    win.SetRect(0, 0, 1920, 1080);

    display.AddHdmiCameraLayout(win);

    if (iHdmi-- > 0)
    {
        win.SetRect(0, 0, 1280, 720);

        display.AddHdmiVideoLayout(CALL_1, win);
    }

    if (iHdmi-- > 0)
    {

        win.SetRect(0, 0, 0, 0);
        // 极小值
        display.AddHdmiVideoLayout(CALL_2, win);
    }

    if (iHdmi-- > 0)
    {
        win.SetRect(-1, -2, 2000, 110);

        // 非法值
        display.AddHdmiVideoLayout(CALL_3, win);
    }

    if (iHdmi-- > 0)
    {
        win.SetRect(1, 1, 1919, 1079);

        display.AddHdmiVideoLayout(CALL_4, win);
    }

    if (iHdmi-- > 0)
    {
        win.SetRect(1, 1, 400, 240);

        display.AddHdmiVideoLayout(CALL_5, win);
    }

    modmedia_SetVideoLayout(display);
}

bool CMediaTest::test_CtrlLocalVideo(bool bOpen)
{
    DisplayMonitor display;
    chRect win(0, 0, 1920, 1080);
    display.AddCameraLayout(win);
    modmedia_CtrlLocalVideo(display, bOpen);
}


bool CMediaTest::test_MultiVideo(int iNumber)
{
    if (iNumber
            && test_StartVideo(CALL_1))
    {
        iNumber--;
        int iNum = GetVideoSize();
        if (iNum != 1)
        {
            VOICE_ERR("Test 1 startVideo fail");
        }
    }

    if (iNumber
            && test_StartVideo(CALL_2))
    {
        iNumber--;
        int iNum = GetVideoSize();
        if (iNum != 2)
        {
            VOICE_ERR("Test 2 startVideo fail");
        }
    }

    if (iNumber
            && test_StartVideo(CALL_3))
    {
        iNumber--;
        int iNum = GetVideoSize();
        if (iNum != 3)
        {
            VOICE_ERR("Test 3 startVideo fail");
        }
    }

    if (iNumber
            && test_StartVideo(CALL_4))
    {
        iNumber--;
        int iNum = GetVideoSize();
        if (iNum != 4)
        {
            VOICE_ERR("Test 4 startVideo fail");
        }
    }

    if (iNumber
            && test_StartVideo(CALL_5))
    {
        iNumber--;
        int iNum = GetVideoSize();
        if (iNum != 5)
        {
            VOICE_ERR("Test 5 startVideo fail");
        }
    }
}

bool CMediaTest::test_KeyframeSync(int iCallId)
{
    etl_NotifyAppEx(true, TALK_MSG_VIDEO_KEY_FRAME_REQUEST, iCallId, 0, 5, "test");
}

bool CMediaTest::GetVideoSize()
{
#ifdef IF_SUPPORT_VIDEO
    YLList< CBaseMedia *> & listProcess = _MediaManager.m_listProcessor;

    for (MediaList::iterator it = listProcess.begin(); it != listProcess.end(); ++it)
    {
        CBaseMedia * pProcessor = *it;
        CVideoManager * pVideo = dynamic_cast<CVideoManager *>(pProcessor);
        if (NULL != pVideo)
        {
            return pVideo->GetVideoNum();
        }
    }
#endif
}

#endif //IS_MEDIA_TEST
