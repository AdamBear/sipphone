#ifndef __TEST_PHONE_T4X_H__
#define __TEST_PHONE_T4X_H__

#include "testphone.h"

#include "../include/hpdev.h"

#define  MAX_STATE_NUM  7
#include "teststate.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "commonapi/stringhelperapi.h"



///////////////////////////CTestPhoneT4X///////////////////////////////////
class CTestPhoneT4X : public CTestPhone
{
    friend class CDlgTestMode;

    friend void HpdevEvcb(void * private_data, HPDEV_EVT * hpdev_evt);


public:
    static CTestPhoneT4X * GetInstance();
    static void DestoryInstance();
    virtual VOID StartEchoTimer();
    virtual VOID StopEchoTimer();
    bool ProcessKeyEvent(int iKeyCode, bool bPressed);//按下和弹起都在这个函数里面处理
    virtual void OnFunkeyPress(int iKey, bool bIsExpKey = false);
    //virtual void OnDigitkeyPress(int iKey);
    void LightOn(int iLight);
    void LightOff(int iLight);
    bool IsLightOn(int iLight);
    void SetUSBState(USB_STATE eUSBState);
    virtual void InitEXPTest(LIGHT_STEP eLightSetp);
    void LightLinekeyLed(LIGHT_STEP eLightSetp);
    virtual void LightLinekeyLedByCross(LIGHT_CROSS_STEP eLightSetp);
    void setCanJumpToEchoTest(bool bCanJump)
    {
        m_bCanJumpToEchoTest = bCanJump;
    }
    bool getCanJumpToEchoTest()
    {
        return m_bCanJumpToEchoTest;
    }
    void resetLCDLevel()
    {
        m_iLCDLevel = 100;
        m_bLightLevelUp = false;
    }

protected:
    CTestPhoneT4X();
    virtual ~CTestPhoneT4X();

protected:
    virtual void InitState();
    virtual void InitSFPState();
    virtual void InitLight();
    virtual bool OnTimer(UINT uTimerID);
    virtual void AllLightOff();
    virtual void ExitLCDTest();
    virtual CTestState * GetNextState()  ;//lcm
    virtual CTestState * CreateStateByType(TEST_STATE eTestState);
    virtual bool IsEchoState() ;
    void StartCircleLightTest();
    // 初始化按键测试内容和顺序
    void InitKeyTestOrder(CTestState * pTestState);

public:
    void StartLightTest();
    void StartLCDTest();

private:
    int         m_iLightTimer;//
    int         m_iLCDTimer;
    int         m_iExpLightTimer;//lcmExp模块的灯
    int         m_iEchoTimer;
    int         m_iLCDLevel;
    int         m_iLightStatus;
    LIGHT_Type m_ExpLightType;
    bool        m_bCircleLight;//lcm循环亮灯
//  TEST_STATE  T4xState[MAX_STATE_NUM];

    bool            m_bCanJumpToEchoTest;

private:
    static CTestPhoneT4X * s_pInst;
};

#define g_pTestPhone CTestPhoneT4X::GetInstance()
enum TEST_STEP_INDEX
{
    TSI_RANDNUM,
    TSI_LCD,
    TSI_USB,
    TSI_KEY,
//     TSI_EXP_KEY,
    TSI_LED,
    TSI_ECHO,
    TSI_EXIT
};

#endif
