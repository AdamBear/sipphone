#ifndef __TEST_EXP_H__
#define __TEST_EXP_H__

#include "lamp/ledlamp/include/modledlamp.h"
#include "exp/include/explogicdefine.h"
#include "test_include.h"
///////////////////////////CTestEXP///////////////////////////////////
class CTestEXP
{
public:
    explicit CTestEXP(UINT uEXPNum);
    virtual ~CTestEXP();

    virtual void LightOnAllEXPByType(LIGHT_Type eType) = 0;
    virtual void LightOffAllEXP() = 0;
    virtual void LightOnFirstEXP(LIGHT_STEP eStep) = 0;
    virtual BOOL OnTimer(UINT uTimerID) = 0;

    // 获取按键灭灯
    virtual void LightOffByKeyIndex(int iKey) = 0;
    // 获取按键测试时的当前按键对应的序号
    virtual int GetKeyIndex(UINT uIndex) = 0;
    // 开始EXP测试
    virtual void StartEXPTest() = 0;
    // 退出EXP测试
    virtual void ExitEXPTest() = 0;
    // 是否需要测试EXP液晶
    virtual bool BeNeedTestLCD() = 0;
    // 绘EXP LCD
    virtual void DrawEXPLCD(int iStep) = 0;
    // EXP LCD的测试步骤
    virtual int  GetEXPLCDMaxStep() = 0;

    virtual void DrawScreenByColor(const EXP_FILL_COLOR& objExpColor, int nIndex = 0);
    virtual void DrawScreenByText(const yl::string& strText, int nIndex = 0);

    UINT GetKeyNum()
    {
        return m_uMaxKeyNum;
    }
    yl::string GetEXPKeyString(int nIndex);

protected:
    void LightByDsskeyModel(int iKeyCode, LIGHT_Type eType);

protected:
    UINT    m_uEXPNum;
    UINT    m_uMaxKeyNum;
};

/////////////////////////EXP BWBase 带屏幕的EXP基类/////////////////////////
class CTestEXPBWBase : public CTestEXP
{
public:
    explicit CTestEXPBWBase(UINT uEXPNum);
    virtual ~CTestEXPBWBase();

    //Start/End
    virtual void StartEXPTest();
    virtual void ExitEXPTest();

    virtual BOOL OnTimer(UINT uTimerID)
    {
        return false;
    }

    //Keys
    virtual int GetKeyIndex(UINT uIndex) = 0;

    //Light
    virtual void LightOnFirstEXP(LIGHT_STEP eStep);
    virtual void LightOnEXP(int iEXPIndex, LIGHT_Type eType);
    virtual void LightOnAllEXPByType(LIGHT_Type eType);
    virtual void LightOffAllEXP();
    virtual void LightOffByKeyIndex(int iKey);

    //Draw LCD
    virtual bool BeNeedTestLCD()
    {
        return true;
    }
    virtual void DrawEXPLCD(int iStep);
    virtual void ShowEXPGray(BYTE byIndex);
    virtual int GetMaxCol()
    {
        return 2;
    }

    // EXP LCD的测试步骤
    virtual int  GetEXPLCDMaxStep()
    {
        return 4;
    }

    virtual void SetLedStatus(int hop, int row, int col, int led_status);

protected:
    int m_nModuleType;
};

///////////////////////////CTestEXP38///////////////////////////////////
class CTestEXP38 : public CTestEXPBWBase
{
public:
    explicit CTestEXP38(UINT uEXPNum);
    virtual ~CTestEXP38();
    virtual int GetKeyIndex(UINT uIndex);
    virtual bool BeNeedTestLCD()
    {
        return false;
    }
    virtual int GetMaxCol()
    {
        return 4;
    }
};

///////////////////////////CTestEXP39///////////////////////////////////
class CTestEXP39 : public CTestEXPBWBase
{
public:
    explicit CTestEXP39(UINT uEXPNum);
    virtual ~CTestEXP39();
    virtual int GetKeyIndex(UINT uIndex);
};

///////////////////////////CTestEXP40///////////////////////////////////
class CTestEXP40 : public CTestEXPBWBase
{
public:
    explicit CTestEXP40(UINT uEXPNum);
    virtual ~CTestEXP40();
    virtual int GetKeyIndex(UINT uIndex);
};

///////////////////////////CTestCM69///////////////////////////////////
class CTestCM69 : public CTestEXPBWBase
{
public:
    explicit CTestCM69(UINT uEXPNum);
    virtual ~CTestCM69();
    virtual int GetKeyIndex(UINT uIndex);
};

#ifdef _EXP_COLOR_SUPPORT
///////////////////////////CTestCM69///////////////////////////////////
class CTestEXP50 : public CTestEXPBWBase
{
public:
    explicit CTestEXP50(UINT uEXPNum);
    virtual ~CTestEXP50();
    virtual int GetKeyIndex(UINT uIndex);

    virtual void DrawScreenByColor(const EXP_FILL_COLOR& objExpColor, int nIndex = 0);
    virtual void DrawScreenByText(const yl::string& strText, int nIndex = 0);
    virtual void SetLedStatus(int hop, int row, int col, int led_status);

    virtual void LightOnEXP(int iEXPIndex, LIGHT_Type eType);
    virtual void LightOffByKeyIndex(int iKey);
    virtual void DrawEXPLCD(int iStep);
    virtual void ExitEXPTest();

    // EXP LCD的测试步骤
    virtual int  GetEXPLCDMaxStep()
    {
        return 10;
    }

};
#endif

#endif //__TEST_EXP_H__

