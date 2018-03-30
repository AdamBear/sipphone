#ifndef __TEST_STATE_H__
#define __TEST_STATE_H__

#include "ETLLib/ETLLib.hpp"
#include "../include/testmode_common.h"
#include "commonapi/stringhelperapi.h"
#include <QTimer>
#include"../include/hpdev.h"
class CTestPhone;
#ifndef IF_USB_SUPPORT
// USB检测相关函数声明
static void SignalHandler(int signo);
#endif
#define  MID_VOLUME  8
static void HpdevEvcb(void * private_data, HPDEV_EVT * hpdev_evt);
///////////////////////////CTestState///////////////////////////////////
class CTestState  : public QObject
{
    Q_OBJECT
public:
    CTestState(CTestPhone * pTestPhone);
    virtual ~CTestState();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false) = 0;
    virtual bool OnDigitkeyPress(int iKey) = 0;
    virtual bool OnTimer(UINT uTimerID) = 0;
    virtual bool DrawScreen() = 0;

    // 进入该状态之后的操作
    virtual void AfterEnterState() = 0;
    virtual void BeforeExitState() {}
    virtual bool isStateLastStep()
    {
        return true;
    }

    void SetTimerID(int iTimerID)
    {
        m_iTimerID = iTimerID;
    }
protected:
    CTestPhone * m_pTestPhone;
    UINT m_iTimerID;
};

///////////////////////////CTestStateInit///////////////////////////////////

///////////////////////////CTestStateEasyTouch///////////////////////////////////
class CTestStateEasyTouch : public CTestState
{
    Q_OBJECT
public:
    CTestStateEasyTouch(CTestPhone * pTestPhone);
    virtual ~CTestStateEasyTouch();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID)
    {
        return FALSE;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
    virtual void BeforeExitState();

private:
};

///////////////////////////CTestStateLCD///////////////////////////////////
class CTestStateLCD : public CTestState
{
    Q_OBJECT
public:
    CTestStateLCD(CTestPhone * pTestPhone);
    virtual ~CTestStateLCD();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID)
    {
        return false;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
    virtual void BeforeExitState();
    virtual bool isStateLastStep();

public slots:
    void OnTimeOut();
private:
    LCD_COLOR_STEP m_eCurrentStep;
    QTimer m_LCDTimer;
};

///////////////////////////CTestStateUSB///////////////////////////////////
class CTestStateUSB : public CTestState
{
    Q_OBJECT
public:
    CTestStateUSB(CTestPhone * pTestPhone);
    virtual ~CTestStateUSB();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID)
    {
        return false;
    }
    virtual bool DrawScreen();
    void DrawUSBString();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

    void SetUSBState(USB_STATE eUSBState);
public slots:
    void AllChecked();

private:
    // 检测USB状态
    void CheckUSBState();

private:
    USB_STATE m_eUSBState;
    bool m_AllChecked;
    bool  m_isrung;
    //HPDEV_HANDLE m_handle;
};

///////////////////////////CTestStateKey///////////////////////////////////
class CTestStateKey : public CTestState
{
public:
    CTestStateKey(CTestPhone * pTestPhone);
    virtual ~CTestStateKey();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey);
    virtual bool OnTimer(UINT uTimerID);
    virtual bool DrawScreen();
    void InitCmpString();
    void AddString(yl::string str);
    yl::string GetCorrectString(int iIndex);
    LPCSTR GetKeyString(int iIndex);
    UINT GetMaxKeyNum()
    {
        return m_uMaxKeyNum;
    }
    void DrawKeyString(bool bCorrect, int iIndex);
    // 进入该状态之后的操作
    virtual void AfterEnterState();

private:
    bool    m_bInputCorrect;
    int     m_iCurrentIndex;
    yl::string  m_strKey[64];//存储字符串
    UINT        m_uMaxKeyNum; // 按键的个数
};
///////////////////////////CTestStateLED///////////////////////////////////
class CTestStateLED : public CTestState
{
public:
    CTestStateLED(CTestPhone * pTestPhone);
    virtual ~CTestStateLED();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID)
    {
        return true ;
    }
    virtual bool DrawScreen();
    virtual void AfterEnterState();
    virtual bool isStateLastStep();

private:
    void   LightLedByStep();
    int m_LedStep;

};

///////////////////////////CTestStateEcho and PreBurnning state///////////////////////////////////
class CTestStateEcho : public CTestState
{
public:
    CTestStateEcho(CTestPhone * pTestPhone);
    virtual ~CTestStateEcho();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID);
    virtual bool DrawScreen();
    void DrawEchoString(ECHO_MODE eMode);
    // 进入该状态之后的操作
    virtual void AfterEnterState();
    virtual void BeforeExitState();

    // 根据当前Echo状态调用VPM接口
    void StartEchoByVPM();
    void StartEchoTest(ECHO_MODE eMode) ;
    // 获取当前回音测试状态
    ECHO_MODE GetEchoType();
    bool ChangeEchoState(int iKey, ECHO_MODE & eMode);
private:
    ECHO_MODE   m_eCurrentMode;
};

///////////////////////////CTestStateExit///////////////////////////////////
class CTestStateExit : public CTestState
{
public:
    CTestStateExit(CTestPhone * pTestPhone);
    virtual ~CTestStateExit();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID)
    {
        return false;
    }
    virtual bool DrawScreen()
    {
        return false;
    }

    // 进入该状态之后的操作
    virtual void AfterEnterState();
};

///////////////////////////CTestStateEXPLight///////////////////////////////////

///////////////////////////CTestStateEXPKey///////////////////////////////////
class CTestStateEXPKey : public CTestState
{
public:
    CTestStateEXPKey(CTestPhone * pTestPhone);
    virtual ~CTestStateEXPKey();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID)
    {
        return false;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

private:
    bool        m_bInputCorrect;
    int     m_iCurrentIndex;
    bool    m_bEXPOk;
};
///////////////////////////CTestStateEXPLCD///////////////////////////////////

///////////////////////////CTestStateEXPExit///////////////////////////////////

///////////////////////////CTestStateRandNum///////////////////////////////////
class CTestStateRandNum : public CTestState
{
public:
    CTestStateRandNum(CTestPhone * pTestPhone);
    virtual ~CTestStateRandNum();

    virtual bool  OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID)
    {
        return false;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
    virtual void BeforeExitState();
};

///////////////////////////CTestStateAudio///////////////////////////////////
class CTestStateAudio : public CTestState
{
public:
    CTestStateAudio(CTestPhone * pTestPhone);
    virtual ~CTestStateAudio();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID)
    {
        return true;
    }
    virtual bool DrawScreen();
    // 进入该状态之后的操作
    virtual void AfterEnterState();
private:

};

///////////////////////////CTestStateEXPLight///////////////////////////////////
class CTestStateEXPLight : public CTestState
{
public:
    CTestStateEXPLight(CTestPhone * pTestPhone);
    virtual ~CTestStateEXPLight();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID)
    {
        return false;
    }
    virtual bool DrawScreen();
    void DrawEXPLightString(LIGHT_STEP eStep);
    void DrawEXPVersionString();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

private:
    LIGHT_STEP  m_eLightStep;
};

///////////////////////////CTestRealStateEXPKey///////////////////////////////////
class CTestRealStateEXPKey : public CTestState
{
public:
    CTestRealStateEXPKey(CTestPhone * pTestPhone);
    virtual ~CTestRealStateEXPKey();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey);
    virtual bool OnTimer(UINT uTimerID)
    {
        return false;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
    void DrawEXPKeyString(bool bCorrect, int iIndex);
private:
    bool    m_bInputCorrect;
    int     m_iCurrentIndex;
    bool    m_bEXPOk;
};

/////////////////////////////CTestStateEXPLCD///////////////////////////////////
class CTestStateEXPLCD : public CTestState
{
public:
    CTestStateEXPLCD(CTestPhone * pTestPhone);
    virtual ~CTestStateEXPLCD();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID)
    {
        return false;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();

private:
    int     m_iCurrentStep;
    bool    m_bEXPOk;
    bool    m_bLCDTestFinished;
};

/////////////////////////////CTestStateEXPExit///////////////////////////////////
class CTestStateEXPExit : public CTestState
{
public:
    CTestStateEXPExit(CTestPhone * pTestPhone);
    virtual ~CTestStateEXPExit();

    virtual bool OnFunkeyPress(int iKey, bool bIsExpKey = false);
    virtual bool OnDigitkeyPress(int iKey)
    {
        return true;
    }
    virtual bool OnTimer(UINT uTimerID)
    {
        return false;
    }
    virtual bool DrawScreen();

    // 进入该状态之后的操作
    virtual void AfterEnterState();
};
#endif //__TEST_PHONE_H__
