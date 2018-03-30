#ifndef __TEST_STATE_H__
#define __TEST_STATE_H__

#include "xwindow-kernel/include/xthreadtimer.h"

class CTestPhone;
///////////////////////////CTestState///////////////////////////////////
class CTestState
{
public:
    explicit CTestState(CTestPhone * pTestPhone);
    virtual ~CTestState();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false) = 0;
    virtual bool DrawScreen() = 0;
    virtual bool OnDigitkeyPress(int iKey);
    virtual void OnEHSInsert()      {};
    virtual void OnEHSRemove()      {};
    // 进入该状态之后的操作
    virtual void AfterEnterState() = 0;
    virtual bool OnMessage(msgObject & msg)
    {
        return false;
    }

protected:
    void ChangeState(TEST_STATE eTestState);

protected:
    CTestPhone   *  m_pTestPhone;
};

///////////////////////////CTestStateInit///////////////////////////////////
class CTestStateInit : public CTestState
{
public:
    explicit CTestStateInit(CTestPhone * pTestPhone);
    virtual ~CTestStateInit();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

private:
    BOOL m_bBlack;
};

///////////////////////////CTestStateLCD///////////////////////////////////
class CTestStateLCD : public CTestState, public xTimerHandler
{
public:
    explicit CTestStateLCD(CTestPhone * pTestPhone);
    virtual ~CTestStateLCD();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

    void OnTimer(xThreadTimer* timer, LPVOID pExtraData);

private:
    LCD_STEP m_eCurrentStep;
    // LCD测试的时间
    xThreadTimer m_timer;
};

///////////////////////////CTestStateKey///////////////////////////////////
class CTestStateKey : public CTestState
{
public:
    explicit CTestStateKey(CTestPhone * pTestPhone);
    virtual ~CTestStateKey();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey);
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

protected:
    yl::string GetKey(int iKey, bool bIsExpKey);

private:
    bool    m_bInputCorrect;
    int     m_iCurrentIndex;
#ifdef _CP920
    bool m_bTestOver;
#endif
};

#ifdef _CP920
///////////////////////////CTestStateAutoTestKey///////////////////////////////////
class CTestStateAutoTestKey : public CTestState
{
public:
    explicit CTestStateAutoTestKey(CTestPhone * pTestPhone);
    virtual ~CTestStateAutoTestKey();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey);
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

protected:
    void AutoTestKeyBoard();

protected:
    enum
    {
        KB_TEST_RESULT_NONE     = 0,
        KB_TEST_RESULT_SUCCESS  = 1,
        KB_TEST_RESULT_FAIL     = 2,
    };

    int m_nResult;
};

///////////////////////////CTestStateBackLight///////////////////////////////////
class CTestStateBackLight : public CTestState, public xTimerHandler
{
public:
    explicit CTestStateBackLight(CTestPhone* pTestPhone);
    virtual ~CTestStateBackLight();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

    // 播放定时器处理
    void OnBackLightTimer(xThreadTimer* timer, LPVOID pExtraData);

private:
    xThreadTimer    m_timerBacklight;
    bool            m_bBackLightOn;
    int             m_nInitBacklightLevel;
};
#endif

///////////////////////////CTestStateEcho///////////////////////////////////
class CTestStateEcho : public CTestState, public xTimerHandler
{
public:
    explicit CTestStateEcho(CTestPhone * pTestPhone);
    virtual ~CTestStateEcho();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool DrawScreen();
    virtual void OnEHSInsert();
    virtual void OnEHSRemove();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

protected:
    // 开始录音计时
    void StartSpeakRecordTimer();
    // 停止录音计时
    void StopSpeakRecordTimer();
    // 录音定时器处理
    void OnSpeakRecordTimer(xThreadTimer* timer, LPVOID pExtraData);

    // 开始播放计时
    void StartSpeakPlayTimer();
    // 停止播放计时
    void StopSpeakPlayTimer();
    // 播放定时器处理
    void OnSpeakPlayTimer(xThreadTimer* timer, LPVOID pExtraData);

private:
    ECHO_MODE   m_eCurrentMode;
    // 正在提示是否跳过EHS测试
    bool        m_bNotify;
    // 录音测试定时器
    xThreadTimer m_timerSpeakRecord;
    // 录音播放定时器
    xThreadTimer m_timerSpeakPlay;
};

///////////////////////////CTestStateExit///////////////////////////////////
class CTestStateExit : public CTestState
{
public:
    explicit CTestStateExit(CTestPhone * pTestPhone);
    virtual ~CTestStateExit();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false)
    {
        return false;
    }
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool DrawScreen()
    {
        return false;
    }

    // 进入该状态之后的操作
    virtual void AfterEnterState();
};

///////////////////////////CTestStateAudio///////////////////////////////////
class CTestStateAudio : public CTestState
{
public:
    explicit CTestStateAudio(CTestPhone * pTestPhone);
    virtual ~CTestStateAudio();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool DrawScreen();
    // 进入该状态之后的操作
    virtual void AfterEnterState();
    void BeforeExitState();

private:
    AUDIO_MODE      m_eCurrentMode;
};

///////////////////////////CTestStateEXPLight///////////////////////////////////
class CTestStateEXPLight : public CTestState, public xTimerHandler
{
public:
    explicit CTestStateEXPLight(CTestPhone * pTestPhone);
    virtual ~CTestStateEXPLight();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

#ifdef _EXP_COLOR_SUPPORT
protected:
    // 彩屏呼吸灯的定时器处理
    void OnTimer(xThreadTimer* timer, LPVOID pExtraData);
#endif

private:
    LIGHT_STEP  m_eLightStep;

#ifdef _EXP_COLOR_SUPPORT
    int         m_iLCDLevel;
    bool        m_bLightLevelUp;
    // 彩屏呼吸灯效果
    xThreadTimer m_timer;
#endif
};

///////////////////////////CTestStateEXPKey///////////////////////////////////
class CTestStateEXPKey : public CTestState
{
public:
    explicit CTestStateEXPKey(CTestPhone * pTestPhone);
    virtual ~CTestStateEXPKey();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

private:
    bool    m_bInputCorrect;
    int     m_iCurrentIndex;
    bool    m_bEXPOk;
};

///////////////////////////CTestStateLED///////////////////////////////////
class CTestStateLED : public CTestState
{
public:
    explicit CTestStateLED(CTestPhone * pTestPhone);
    virtual ~CTestStateLED();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool DrawScreen();
    virtual void AfterEnterState();

protected:
    void LightLedByStep();
    int  m_LedStep;

};

///////////////////////////CTestStateEXPLCD///////////////////////////////////
class CTestStateEXPLCD : public CTestState, public xTimerHandler
{
public:
    explicit CTestStateEXPLCD(CTestPhone * pTestPhone);
    virtual ~CTestStateEXPLCD();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

    void OnTimer(xThreadTimer* timer, LPVOID pExtraData);

private:
    int     m_iCurrentStep;
    bool    m_bEXPOk;
    bool    m_bLCDTestFinished;
    xThreadTimer m_timer;
};

///////////////////////////CTestStateEXPExit///////////////////////////////////
class CTestStateEXPExit : public CTestState
{
public:
    explicit CTestStateEXPExit(CTestPhone * pTestPhone);
    virtual ~CTestStateEXPExit();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
};

#if JOYCE_TEL_BUGFREE_19348
///////////////////////////CTestStateSN///////////////////////////////////
class CTestStateSN : public CTestState
{
public:
    explicit CTestStateSN(CTestPhone * pTestPhone);
    virtual ~CTestStateSN();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

private:
    // 获取客户特制SN内容
    yl::string GetSN();
};
#endif // JOYCE_TEL_BUGFREE_19348

///////////////////////////CTestStateRPS///////////////////////////////////
class CTestStateRPS : public CTestState
{
public:
    explicit CTestStateRPS(CTestPhone * pTestPhone);
    virtual ~CTestStateRPS();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
};

#ifdef _CP
///////////////////////////CTestStateCPEcho///////////////////////////////////
class CTestStateCPEcho : public CTestState, public xTimerHandler
{
public:
    explicit CTestStateCPEcho(CTestPhone * pTestPhone);
    virtual ~CTestStateCPEcho();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey);
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

    // 根据当前Echo状态调用VPM接口
    // bReset某个回音测试时是否需要重置成默认状态(切换模式或play到record)
    void StartEchoByVPM();
    bool ChangeEchoState(int iKey);
    bool ChangeChannel();
    void ChangeLightState(LED_STATE eForceState = LED_STATE_NONE);

    // 开始播放计时
    void StartSpeakPlayTimer();

    void OnPlayTimerEnd(xThreadTimer* timer, LPVOID pExtraData);
    void OnMicIndexTimerEnd(xThreadTimer* timer, LPVOID pExtraData);

private:
    // 录放操作
    void ProcessRecordPlay();
    void BeforeExit();

    // 开始录音计时
    void StartSpeakRecordTimer();
    // 停止录音计时
    void StopSpeakRecordTimer();
    // 录音定时器处理
    void OnSpeakRecordTimer(xThreadTimer* timer, LPVOID pExtraData);

private:
    LED_STATE   m_eLEDState;
    ECHO_MODE   m_eCurrentMode;
    ECHO_STATE  m_eCurrentState;
    bool        m_bMute;
    UINT        m_uMicIndex;
    UINT        m_uPlayTime;
    xThreadTimer    m_timerPlayTime;
    xThreadTimer    m_timerMicIndex;
    xThreadTimer    m_timerSpeakRecord;

};

///////////////////////////CTestStateCPEcho///////////////////////////////////
class CTestStateCPAudio : public CTestState
{
public:
    explicit CTestStateCPAudio(CTestPhone * pTestPhone);
    virtual ~CTestStateCPAudio();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey);
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
    bool ChangeChannel();
    void ChangeLightState(LED_STATE eForceState = LED_STATE_NONE);

private:
    void BeforeExit();

private:
    LED_STATE   m_eLEDState;
    UINT        m_uMicIndex;
    ECHO_MODE   m_eCurrentMode;
};

///////////////////////////CTestStateCPLED///////////////////////////////////
class CTestStateCPLED : public CTestStateLED
{
public:
    explicit CTestStateCPLED(CTestPhone * pTestPhone);
    virtual ~CTestStateCPLED();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }

#ifdef _CP920
    virtual bool DrawScreen();
#endif

};

///////////////////////////CTestStateFinish///////////////////////////////////
class CTestStateFinish : public CTestState
{
public:
    explicit CTestStateFinish(CTestPhone * pTestPhone);
    virtual ~CTestStateFinish();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
};
///////////////////////////CTestStateFinish///////////////////////////////////
class CTestStateIP : public CTestState
{
public:
    explicit CTestStateIP(CTestPhone* pTestPhone);
    virtual ~CTestStateIP();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
private:
    yl::string strIp;
};

///////////////////////////CTestStateResult///////////////////////////////////
class CTestStateResult : public CTestState
{
public:
    explicit CTestStateResult(CTestPhone* pTestPhone);
    virtual ~CTestStateResult();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
private:
    yl::string m_strTestResult;
};
#endif

#ifdef IF_USB_SUPPORT
///////////////////////////CTestStateUSB///////////////////////////////////
class CTestStateUSB : public CTestState
#ifdef _CP920
    , public xTimerHandler
#endif
{
public:
    explicit CTestStateUSB(CTestPhone * pTestPhone);
    virtual ~CTestStateUSB();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }

    virtual bool DrawScreen();
    void OnUSBStateChange(bool bIn);
    bool IsUSBCheckSuccess()
    {
        return m_bUSBCheckSuccess;
    }
    void SetUSBCheckStatus(bool bStatus);
    // 进入该状态之后的操作
    virtual void AfterEnterState();

#ifdef _CP920
    void OnCheckSuccessed(xThreadTimer* timer, LPVOID pExtraData);
#endif

private:
    yl::string  m_strDisp;
    bool        m_bUSBCheckSuccess; // USB是否插入
#ifdef _CP920
    xThreadTimer    m_timerAutoTestNext;
#endif

};
#endif //IF_USB_RECORD

#ifdef IF_SUPPORT_WIFI
///////////////////////////CTestStateWifi///////////////////////////////////
class CTestStateWifi : public CTestState, public xTimerHandler
{
public:
    explicit CTestStateWifi(CTestPhone * pTestPhone);
    virtual ~CTestStateWifi();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnMessage(msgObject & msg);
    virtual bool DrawScreen();
    // 进入该状态之后的操作
    virtual void AfterEnterState();
    void ResumeWifiStatus();
    void OnWifiSignalOkSuccessed(xThreadTimer* timer, LPVOID pExtraData);

private:
    yl::string      m_strDisp;
    bool            m_bOriginWifiEnable;
    xThreadTimer    m_timerAutoTestNext;
};
#endif // IF_SUPPORT_WIFI

///////////////////////////CTestStateView///////////////////////////////////
class CTestStateView : public CTestState
{
public:
    explicit CTestStateView(CTestPhone * pTestPhone);
    virtual ~CTestStateView();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);

    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
};

#if IS_COLOR
///////////////////////////CTestStateColorLCD///////////////////////////////////
class CTestStateColorLCD : public CTestState, public xTimerHandler
{
public:
    explicit CTestStateColorLCD(CTestPhone * pTestPhone);
    virtual ~CTestStateColorLCD();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

    void OnTimer(xThreadTimer* timer, LPVOID pExtraData);

private:
    LCD_COLOR_STEP m_eCurrentStep;

    xThreadTimer m_xTimer;
    bool m_bLightLevelUp;
    int  m_iLCDLevel;

};

///////////////////////////CTestStateBlueTooth///////////////////////////////////
class CTestStateBlueTooth : public CTestState
{
public:
    explicit CTestStateBlueTooth(CTestPhone * pTestPhone);
    virtual ~CTestStateBlueTooth();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool DrawScreen();
    virtual void AfterEnterState();
    virtual bool OnMessage(msgObject & msg);

};
#endif //IS_COLOR

#endif //__TEST_PHONE_H__
