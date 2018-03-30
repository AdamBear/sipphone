#ifndef __TEST_PHONE_H__
#define __TEST_PHONE_H__

#include "ETLLib/ETLLib.hpp"
#include "../include/testmode_common.h"
#include "interfacedefine.h"
#include <QtCore/QObject>
#include "exp/include/modexp.h"

#define MID_VOLUME 8

typedef YLVector<int>           vec_Int;
typedef YLVector<TEST_STATE>    vec_Teststate;

class CAutoVolumeBar;
///////////////////////////CTestPhone///////////////////////////////////
class CTestState;
class CTestEXP;

class CTestPhone : public QObject
{
    Q_OBJECT

public:
    CTestPhone();
    virtual ~CTestPhone();

    // 测试话机初始化
    void TestInitial();
    // 测试整机免提音质话机初始化
    void TestFullVoiceInit();
    //老化测试
    void TestPreBurningInit();
    //半成品测试初始化
    void TestSFPInit();

    /////////////////////////////需要子类重载的函数//////////////////////////////
    //lcm 状态的初始化
    virtual void InitState() =
        0;//不同机型可能需要测试不一样的状态，T46和T48就不一样
    virtual void InitSFPState() = 0;//半成品测试
    // 灯的初始化
    virtual void InitLight() = 0;//不同机型不一样的灯
    virtual CTestState * GetNextState() = 0 ;//没有将测试类型存储在基类中
    virtual CTestState * CreateStateByType(TEST_STATE eTestState) = 0;
    virtual bool IsEchoState() = 0;//当前状态是否是回音测试，
    // 定时器响应
    virtual bool OnTimer(UINT uTimerID) = 0;
    virtual void LightOn(int iLight) = 0;
    virtual void LightOff(int iLight) = 0;
    // 灭所有灯
    virtual void AllLightOff() = 0;
    virtual void LightLinekeyLed(LIGHT_STEP eLightSetp) = 0;
    virtual void LightLinekeyLedByCross(LIGHT_CROSS_STEP eLightSetp) = 0;
    virtual void ExitLCDTest() = 0;
    //
    virtual void InitEXPTest(LIGHT_STEP eLightSetp) {};
    virtual VOID StartEchoTimer() {};
    virtual VOID StopEchoTimer() {};
    // 调用m_pTestState对应的接口，将事件传递
    virtual void OnFunkeyPress(int iKey,
                               bool bIsExpKey = false) = 0 ; //和具体状态相关，写不死
    virtual void  OnDigitkeyPress(int iKey);
    // 退出EchoTest的后续处理
    void AfterEchoState();
    void CloseBreathLCD();

    // 状态的迁移
    void ChangeState(CTestState * pTestState,    int  TSIndex = -1)  ;

    // 对EXP的操作
    void InitEXPModule();
    void UpdateTestEXP();

    //EXP_TYPE CheckEXPType();
    int SetTimerEvent(int iTimeOut);
    void StopTimerEvent(int iTimeID);
    void PlayLocalDTMF(int iKey);

    //EXP Test
    //进入EXP测试模式
    void StartEXPTest();
    //中途拔出扩展台
    void AbortEXPTest();
protected:
    virtual void timerEvent(QTimerEvent * event);

public:
    CTestEXP  * m_pTestEXP;   // EXP模块
    int         m_iEXPHandle; // 控制EXP灯的句柄，其实就是EXP的句柄

protected:

    vec_Int         m_vec_iLightIndex;          //  灯索引容器
    vec_Teststate   m_vec_Teststate;            //  测试状态容器
    CTestState * m_pTestState; // 当前所在的测试状态
    int         m_iLedStatus; // 灯的状态
    UINT        m_uMaxKeyNum; // 按键的个数
    UINT        m_uEXPNum;    // EXP模块的个数
    UINT        m_uEXPColNum; // 单个EXP的列数,EXP40为2列
    UINT       m_ulightNum;//单个扩展台的灯个数
    EXP_TYPE    m_eEXPType;   // EXP模块的类型
    bool        m_bLightLevelUp;
    int         m_StateStep;//lcm当前进行到第几个state
    bool      m_isInEXPTest;//当前是够处于EXP测试状态

};

#endif //__TEST_PHONE_H__
