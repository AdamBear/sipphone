#ifndef __TEST_EXP_H__
#define __TEST_EXP_H__

#include "ETLLib/ETLLib.hpp"
#include "interfacedefine.h"
#include "testmode_common.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "exp/include/modexp.h"
#include "dsskey/src/dsskey_common.h"


#include "testmode_common.h"

#include "testphonet4x.h"

#include "dsskey/include/moddsskey.h"
///////////////////////////CTestEXP//////////////////////////////////
static short EXP40KeyIndex[DEV_EXP_KEY_NUM_40] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18,
                                                  1, 3, 5, 7, 9, 11, 13, 15, 17, 19,
                                                 };
static short EXP39KeyIndex[DEV_EXP_KEY_NUM_39] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18,
                                                  1, 3, 5, 7, 9, 11, 13, 15, 17, 19,
                                                 };
static short EXP38KeyIndex[DEV_EXP_KEY_NUM_38] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18,  1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37,
                                                  20, 22, 24, 26, 28, 30, 32, 34, 36
                                                 };
class CTestEXP
{
public:
    CTestEXP(UINT uEXPNum, EXP_TYPE     eEXPType, UINT ulightNum,   UINT uMaxKeyNum);
    virtual ~CTestEXP();
    void LightOnAllEXP();
    void LightOffAllEXP();
    void LightOnFirstEXP(LIGHT_STEP eStep);

    bool BeNeedTestEXPLCD()
    {
        return    m_isNeedTestEXPLCD;
    }
    // 获取按键测试时的当前按键对应的序号
    //根据灯的类型来点亮整个扩展台
    void LightEXP(int iEXPNum, LIGHT_Type iLightType);
    void LightCtrl(int iEXPKey, LIGHT_Type eLEDType);
    void LightOffByKeyIndex(int iKey);
    void LightOnByKeyIndex(int iKey, LIGHT_Type iLightType);
    int GetKeyIndex(UINT uIndex);
    int GetEXPkeyNum()
    {
        return m_ulightNum;/*40的两个按键不使用，但是还是保留数目，实际使用的为-2*/
    }
    chString GetEXPKeyString(UINT uIndex);
    void DrawEXPLCD(int iStep);
    void ShowEXPGray(BYTE byIndex);
    void ShowEXPLogo(BYTE byIndex);
private:
    //lcm根据扩展台类型初始化dsskeymodule类型
    void InitDsskey_Module_Type();

public:
    UINT    m_uEXPNum;
    UINT    m_uEXPColNum;
    UINT   m_ulightNum;
    EXP_TYPE     m_eEXPType;//当前扩展台类型
    UINT     m_uMaxKeyNum; // 按键的个数
    bool    m_isNeedTestEXPLCD;
    DSSKEY_MODULE_TYPE m_eDssKeyModuleType;
};

#endif //__TEST_EXP_H__
