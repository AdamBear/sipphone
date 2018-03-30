#ifndef __TEST_PHONE_H__
#define __TEST_PHONE_H__


#include "xwindow/xWindow.h"
#include "uimanager/basedialog.h"
#include "common/common_data_define.h"
#include "test_include.h"
#include "uimanager/autovolumebar.h"

#define KEY_NUM 39
#define MID_VOLUME 8
#define MAX_VOLUME 15
#define MAX_KEY_NUM 49
///////////////////////////CTestPhone///////////////////////////////////
class CTestState;
class CTestEXP;
class CAutoVolumeBar;
class CTestPhone : public xTimerHandler
{
public:
    CTestPhone();
    virtual ~CTestPhone();

    // 测试话机初始化
    void TestInitial();

    // 测试半成品初始化
    virtual void TestSFPInitial();

    // 测试整机免提音质话机初始化
    virtual void TestFullVoiceInit();

    // 测试MIC老化模式初始化
    virtual void TestPreburningInit();

    //手动测试按键初始化
    virtual void TestManaualKeyInit();

#ifdef IF_SUPPORT_WIFI
    // 测试wifi初始化
    virtual void TestWifiInit();
#endif

#ifdef _CP
    // 检测防呆报警标志
    void CheckFlagPalyTone();
    // 查看防呆报警标志
    virtual void TestResultInit();
    // 设置完整测试标志
    void SetFullTestFlag(bool bFull);
    // 设置完整测试标志
    bool GetFullTestFlag() {
        return m_bFullFlag;
    }
#endif

    virtual void Show();
    virtual bool OnKeyRelease();

    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);

#ifdef IF_USB_SUPPORT
    virtual void OnUSBStateChange(bool bIn);
#endif // IF_USB_RECORD

    void OnEHSInsert();
    void OnEHSRemove();

    /////////////////////////////需要子类重载的函数//////////////////////////////
    // 灯的初始化
    virtual void InitLight() = 0;
    // 按键字符的初始化
    virtual void InitCmpString() = 0;
    // 在Init状态时，根据按键跳转到其他状态
    virtual TEST_STATE MapStateOfKey(int iKey, bool bEXPKey = false) = 0;
    // 填充整个屏幕
    virtual void FillFullRect(bool bBlack);
    // 填充整个屏幕
    virtual void FillPicture(const yl::string& strPic) {}
    // 对液晶的测试(分阶段测试)
    virtual void TestLCDByStep(LCD_STEP eStep) = 0;
    // 根据按键输入的状态画出对应的string
    virtual void DrawKeyString(bool bCorrect, int iIndex) = 0;
    // 根据按键更改Echo的状态
    virtual bool ChangeEchoState(int iKey, ECHO_MODE & eMode) = 0;
    // 根据Echo的状态画出对应的string
    virtual void DrawEchoString(ECHO_MODE eMode) = 0;
    // 根据Echo的状态开启对应的EchoTest
    virtual void StartEchoTest(ECHO_MODE eMode, UINT iExtData = 0);
    // 画中间文字
    virtual void DrawContentString(LPCTSTR lpszTest, xColor color = xColor(0x00, 0x00, 0x00));
    // 画RPS的界面
    virtual void DrawRpsString(bool bRpsStatus, LPCTSTR lpszRandnum, LPCSTR lpszCert) {}
    // 画RPS状态数据
    virtual void DrawStatusViewString(LPCSTR lpszTest) {}
    // 根据EXP灯的状态画出对应的string
    virtual void DrawEXPLightString(LIGHT_STEP eStep) = 0;
    // 根据EXP按键处输入的状态画出对应的string
    virtual void DrawEXPKeyString(bool bCorrect, int iIndex) = 0;
    // 根据EXP测试通过对应的string
    virtual void DrawEXPTestString(LPCSTR lpszTest) = 0;
    // 灭所有灯
    virtual void AllLightOff() = 0;
    //　播按键音
    virtual void PlayLocalDTMF(int iKey);

    // 灯模式
    virtual void ChangeAllLight(LED_STATE eState) {};

    // 是否是下一步的按键
    virtual bool IsNextStateKeyPress(int iKey) {
        return (PHONE_KEY_HANDFREE == iKey);
    }

#if JOYCE_TEL_BUGFREE_19348
    // 显示客户特制的SN
    virtual void DrawSN(LPCSTR lpszSN);
#endif

    // T42/T41 刷新图标
    virtual void DrawAllIcon(bool bShow)                {}

    virtual int  GetLedMaxStep() {
        return 2;
    }

    //////////////////////////////////////////////////////////////////////////////
#ifdef _CP920
    virtual void KeyboardAutoTestInit()                 {}
    virtual bool KeyboardAutoTest()
    {
        return false;
    }
    virtual void ShowErrorKeys(bool bShow)              {}
#endif

    void OnMessage(msgObject & msg);

    bool onKeyPressed(int key, bool bExp);

    void SetTestMethod(int iTestMethod);

    int GetTestMethod()
    {
        return m_iTestMethod;
    }

    // 调用m_pTestState对应的接口，将事件传递
    virtual bool OnFunckeyPress(int iKey, bool bEXP = false);
    bool OnDigitkeyPress(int iKey);

    // 获取按键测试时的当前按键对应的string
    LPCSTR GetKeyString(int iIndex);
    // 退出EchoTest
    void ExitEchoTest();
    // 按键的个数
    UINT GetMaxKeyNum()
    {
        return m_uMaxKeyNum;
    }
    // 状态的迁移
    void ChangeState(CTestState * pTestState);
    // 判断灯是否点亮
    bool IsLightOn(int eIndex);
    // 获取累加的输入正确的按键字符
    yl::string GetCorrectString(int iIndex);

    // 对EXP的操作
    void InitEXPModule();
    void UpdateTestEXP();
    DEVICE_TYPE_DEF CheckEXPType();
    DEVICE_TYPE_DEF GetEXPTypeFromDriver(int iEXPIndex);
    bool EXPKeyPress(int iKey);

    // 设置免提模式
    virtual bool SetSpeakerMode(int iVolume);
    // 回音测试音量调节
    bool ChangeSpeakerVolume(bool bIncrease);
    // 绘制音量条
    void DrawVolumeBar(bool bShow = true);
    // 退出
    bool ExitSpeakerVolume();

    // 记录EHS连接状态
    void SetEHSStatus(bool bInserted)
    {
        m_bIsEHSInserd = bInserted;
    }
    // 获取EHS连接状态
    bool IsEHSSInserted()
    {
        return m_bIsEHSInserd;
    }
    bool IsSkipEHSTest()
    {
        return m_bSkipEHSTest;
    }
    void SetSkipEHSTest(bool bSkip)
    {
        m_bSkipEHSTest = bSkip;
    }
    virtual void LightLinekeyLed(LIGHT_STEP eLightSetp) {};

    void SetEchoMode(ECHO_MODE eMode)
    {
        m_eEchoMode = eMode;
    }
    ECHO_MODE GetEchoMode()
    {
        return m_eEchoMode;
    }

    virtual void SetTestModeEndFlag() {};
#if IF_BUG_20900
    BOOL OnDelayStartMacdTimer(msgObject & msg);
#endif

    bool GetHasLedLight() {
        return m_bHasLed;
    }

    BOOL GetISSFP() {
        return (TST_SFP == m_iTestMethod);
    }

public:
    CTestEXP    *   m_pTestEXP;   // EXP模块
    chMutex         m_objIOMutex; // 用于对灯驱动的同步

protected:
    yl::string      m_strKey[MAX_KEY_NUM]; // 按键对应的字符
    CTestState   *  m_pTestState; // 当前所在的测试状态
    UINT            m_iLedStatus; // 灯的状态
    DSK_TARGET_T    m_hVPMThread; // VPM线程句柄
    UINT            m_uMaxKeyNum; // 按键的个数
    UINT            m_uEXPNum;    // EXP模块的个数
    DEVICE_TYPE_DEF m_eEXPType;   // EXP模块的类型
    bool            m_bHasEXP;    // 是否有EXP
    bool            m_bHasLed;    // 是否有LED灯
    CAutoVolumeBar * m_pVolumeBar; // 音量调节
    int             m_iCurrentVolume; // 记录当前音量值
#if IF_BUG_20900
    xThreadTimer    m_timerDelayStartMacd; // 延时开启Macd
#endif
    bool            m_bIsEHSInserd; // EHS是否已经接上
    bool            m_bSkipEHSTest; // 是否跳过EHS测试
    ECHO_MODE       m_eEchoMode;
    int             m_iTestMethod;
#ifdef _CP
    bool            m_bFullFlag; // 完整测试标志
#endif
};

#endif //__TEST_PHONE_H__
