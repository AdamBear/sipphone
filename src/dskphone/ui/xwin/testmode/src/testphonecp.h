#ifndef __TEST_PHONE_CP_H__
#define __TEST_PHONE_CP_H__

#ifdef _CP
#include "testphonet66.h"
///////////////////////////CTestPhoneCP///////////////////////////////////
#include "pcserver.h"

#define MIC_ALL 0x1f

class CTestPhoneCP : public CTestPhoneT66
{
public:
    CTestPhoneCP();
    virtual ~CTestPhoneCP();

    virtual void TestFullVoiceInit();
    virtual void TestPreburningInit();
    virtual bool IsNextStateKeyPress(int iKey)
    {
        return (PHONE_KEY_OK == iKey);
    }

    virtual void InitLight();
    virtual void InitCmpString();
    virtual TEST_STATE MapStateOfKey(int iKey, bool bEXPKey = FALSE);
    virtual bool ChangeEchoState(int iKey, ECHO_MODE & eMode)
    {
        return FALSE;
    }
    virtual void StartEchoTest(ECHO_MODE eMode, UINT iExtData = 0);
    virtual void LightLinekeyLed(LIGHT_STEP eLightSetp);
    virtual void AllLightOff();

    // 设置免提模式
    virtual bool SetSpeakerMode(int iVolume);

    // 灯模式
    virtual void ChangeAllLight(LED_STATE eState);

    // 按键事件传递
#ifdef _CP920
    virtual void KeyboardAutoTestInit();
    virtual bool KeyboardAutoTest();
    virtual void ShowErrorKeys(bool bShow);
#endif
    virtual bool OnChannelKeyPress(int iKey);
    virtual bool OnKeyRelease();

    // 刷新灯的状态
    virtual void OnFlushLight();

    void AllLightCtrl(LED_STATE eState);

    void OnExitKeyTimer(xThreadTimer* timer, LPVOID pExtraData);

    virtual void SetTestModeEndFlag();

#ifdef _CP920
    typedef struct _ST_KEY_TEST_DATA
    {
        int nRawData;
        int nDelta;
        bool bSuccess;

        _ST_KEY_TEST_DATA()
            : nRawData(0)
            , nDelta(0)
            , bSuccess(false)
        {
        }
    } ST_KEY_TEST_DATA;

    typedef YLHashMap<int, ST_KEY_TEST_DATA> MAP_KEY_TEST_DATA;

    bool IsKeyTestSucceed(int nRawKey);
#endif

    virtual void DrawStatusViewString(LPCSTR lpszTest);

private:
    xThreadTimer    m_timerExitKey;
#ifdef _CP920
    MAP_KEY_TEST_DATA   m_mapKeyTestData;
    int             m_nRoleDataTole;
    int             m_nDeltaTole;
    int             m_nDeltaBase;
#endif
    CPCServer       m_PCServer;
};

#endif //_CP

#endif //__TEST_PHONE_CP_H__
