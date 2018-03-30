#ifndef __TEST_PHONE_T66_H__
#define __TEST_PHONE_T66_H__

#include "testphone.h"
///////////////////////////CTestPhoneT66///////////////////////////////////
#define T66_LINEKEY_NUM             8
#define T66_LINEKEY_GREEN           0
#define T66_LINEKEY_RED             1
#define T66_LINEKEY_ORANGE          2
#define T66_FUNCTION_LIGHT_BEGIN    m_iLineNum*3

class CTestPhoneT66 : public CTestPhone
{
public:
    CTestPhoneT66();
    virtual ~CTestPhoneT66();

    virtual void InitLight();
    virtual void InitCmpString();
    virtual TEST_STATE MapStateOfKey(int iKey, bool EXPKey = false);
    virtual void TestLCDByStep(LCD_STEP eStep);
    virtual void DrawKeyString(bool bCorrect, int iIndex);
    virtual bool ChangeEchoState(int iKey, ECHO_MODE & eMode);
    virtual void DrawEchoString(ECHO_MODE eMode);
    virtual void StartEchoTest(ECHO_MODE eMode, UINT iExtData = 0);
    virtual void DrawRpsString(bool bRpsStatus, LPCTSTR lpszRandnum, LPCSTR lpszCert);
    virtual void DrawStatusViewString(LPCSTR lpszTest);
    virtual void DrawEXPLightString(LIGHT_STEP eStep);
    virtual void DrawEXPKeyString(bool bCorrect, int iIndex);
    virtual void DrawEXPTestString(LPCSTR lpszTest);
    virtual void AllLightOff();
    virtual void LightLinekeyLed(LIGHT_STEP eLightSetp);

    //全屏提示用字体
    virtual int GetNoteTextFont(int nTextType);
    // 对灯的控制
    virtual void FlushLightIndex();
    // 刷新灯的状态
    virtual void OnFlushLight();

    void LightOn(int iIndex);
    void LightOff(int iIndex);
    void StartLightTest();
    void AllLightOn();
    void StartEchoLightTest();
    void AllLightOn(int mLineColor, bool bFuncKey = true);
    void TurnLightAllOff(bool bFuncKey = true);
    void TurnLightOn(int iIndex);
    void TurnLightOff(int iIndex);
    void DoLight();

    void OnLightTimer(xThreadTimer* timer, LPVOID pExtraData);
    void OnEchoLightTimer(xThreadTimer* timer, LPVOID pExtraData);

    // 新需求led测试阶段专用
    virtual void TestAllLightOn(int mLineColor, bool bFuncKey = true);

protected:
    // 获取exp的信息
    void GetEXPInfo(yl::string& strHardware, yl::string& strFirmware);


    // 获取exp软件版本号
    yl::string GetEXPHardware();
    // 获取exp硬件版本号
    yl::string GetEXPFirmware();

protected:
    xThreadTimer    m_timerLight;
    xThreadTimer    m_timerEchoLight;
    int             m_iLightTimer;
    LIGHT_STEP  m_eLightType;
    YLVector<int>   m_vecLightIndex;
    int         m_iFlashIndex;
    int         m_iLineNum;
};

#endif //__TEST_PHONE_T23_H__
