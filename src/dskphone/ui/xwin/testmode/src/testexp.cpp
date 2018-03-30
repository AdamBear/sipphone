#include "test_include.h"
#include "testmodeinterface.h"
#include "exp/include/modexp.h"

//iExpNum 扩展台个数，iLedStatus 灯状态
extern "C" void devicelib_RefreshExpLED(int iExpIndex, int iLedStatus);

///////////////////////////CTestEXP///////////////////////////////////
CTestEXP::CTestEXP(UINT uEXPNum)
{
    m_uEXPNum = uEXPNum;
}

CTestEXP::~CTestEXP()
{

}

yl::string CTestEXP::GetEXPKeyString(int nIndex)
{
    if (nIndex < 0 || nIndex > m_uMaxKeyNum)
    {
        return "";
    }

    yl::string strRetVal = "";
    yl::string strTmp = "";
    for (int i = 0; i < nIndex; ++i)
    {
        strTmp = commonAPI_FormatString("%d ", i + 1);
        strRetVal += strTmp;
    }

    return strRetVal;
}

void CTestEXP::LightByDsskeyModel(int iKeyCode, LIGHT_Type eType)
{
    int iDsskey = dsskey_GetDsskeyInfoByKeyIndex(iKeyCode);
    exp_SetExpLEDStatus(iDsskey, eType);
}

void CTestEXP::DrawScreenByColor(const EXP_FILL_COLOR& objExpColor, int nIndex/* = 0*/)
{

}

void CTestEXP::DrawScreenByText(const yl::string& strText, int nIndex/* = 0*/)
{
}

////////////////////////EXP BWBase 带屏幕的EXP基类/////////////////////////

CTestEXPBWBase::CTestEXPBWBase(UINT uEXPNum) : CTestEXP(uEXPNum)
{
    m_uMaxKeyNum = 0;
    m_nModuleType = 0;
}

CTestEXPBWBase::~CTestEXPBWBase()
{
    LightOffAllEXP();
}

void CTestEXPBWBase::LightOnAllEXPByType(LIGHT_Type eType)
{
    for (int i = 0; i < (int)m_uEXPNum; ++i)
    {
        LightOnEXP(i, eType);
    }
}

void CTestEXPBWBase::LightOffAllEXP()
{
    LightOnAllEXPByType(LT_OFF);
}

void CTestEXPBWBase::LightOnFirstEXP(LIGHT_STEP eStep)
{
    LIGHT_Type eType = LT_OFF;

    switch (eStep)
    {
    case LS_GREEN:
        eType = LT_GREEN_ON;
        break;
    case LS_RED:
        eType = LT_RED_ON;
        break;
    case LS_ORANGEONE:
        eType = LT_RED_GREEN_ON;
        break;
    default:
        break;
    }

    LightOnEXP(0, eType);

#ifndef WIN32
    SetLedStatus(1, 0, 0, eType);
#endif
}

void CTestEXPBWBase::LightOffByKeyIndex(int iKey)
{
#ifndef WIN32
    int iEXPIndex = GetEXPIndex(iKey);
    int iEXPKeyIndex = GetEXPKeyIndex(iKey);

    if (iEXPKeyIndex < (int)m_uMaxKeyNum)
    {
        int row = iEXPKeyIndex / GetMaxCol();
        int col = iEXPKeyIndex % GetMaxCol();
        SetLedStatus(iEXPIndex, row, col, LT_OFF);
    }
#endif
}

void CTestEXPBWBase::LightOnEXP(int iEXPIndex, LIGHT_Type eType)
{
#ifndef WIN32
    const int iLineMax = 10;

    for (int iColIndex = 0; iColIndex < GetMaxCol(); iColIndex++)
    {
        for (int iLineIndex = 0; iLineIndex < iLineMax; iLineIndex++)
        {
            SetLedStatus(iEXPIndex, iLineIndex, iColIndex, eType);
        }
    }
#endif
}

void CTestEXPBWBase::StartEXPTest()
{
    // nothing to do now
}

void CTestEXPBWBase::ExitEXPTest()
{
    // 将灰度刷为正常状态，避免logo大小不够
    ShowEXPGray(EXP_TM_WHITE);
}

void CTestEXPBWBase::DrawEXPLCD(int iStep)
{
    switch (iStep)
    {
    case 0:
        {
            // 全黑
            ShowEXPGray(EXP_TM_BLACK);
        }
        break;
    case 1:
        {
            // 全白
            ShowEXPGray(EXP_TM_WHITE);
        }
        break;
    case 2:
        {
            // 灰阶图
            ShowEXPGray(EXP_TM_GRAY0);
        }
        break;
    case 3:
        {
            // 其他图片
            ShowEXPGray(EXP_TM_PIC0);
        }
        break;
    default:
        break;
    }
}


void CTestEXPBWBase::ShowEXPGray(BYTE byIndex)
{
#ifndef WIN32
    exp_helper_set_lcd_testmode(exp_GetExpHandle(), 0, byIndex);
#endif
}

void CTestEXPBWBase::SetLedStatus(int hop, int row, int col, int led_status)
{
    exp_helper_set_led_status(exp_GetExpHandle(), hop, row, col, led_status);
}

///////////////////////////CTestEXP38///////////////////////////////////
// 扩展台按键1,3~19对应灯28,29~37,按键2,4~18对应灯19,20~27,按键20,22~38对应灯9,10~18,按键,21,23~37对应灯0,1~8
static BYTE EXP38KeyIndex[38] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22,
                                 24, 26, 28, 30, 32, 34, 36, 38, 21, 23, 25, 27, 29, 31, 33, 35, 37
                                };

CTestEXP38::CTestEXP38(UINT uEXPNum) : CTestEXPBWBase(uEXPNum)
{
    m_uMaxKeyNum = 38;
    m_nModuleType = DMT_EXP38_MODULE;
}

CTestEXP38::~CTestEXP38()
{

}

int CTestEXP38::GetKeyIndex(UINT uIndex)
{
    if (uIndex >= m_uMaxKeyNum)
    {
        return -1;
    }

    return EXP38KeyIndex[uIndex];
}
///////////////////////////CTestEXP39///////////////////////////////////
static BYTE EXP39KeyIndex[20] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

CTestEXP39::CTestEXP39(UINT uEXPNum) : CTestEXPBWBase(uEXPNum)
{
    m_uMaxKeyNum = 20;
    m_nModuleType = DMT_EXP39_MODULE;
}

CTestEXP39::~CTestEXP39()
{
}
int CTestEXP39::GetKeyIndex(UINT uIndex)
{
    if (uIndex >= m_uMaxKeyNum)
    {
        return -1;
    }

    return EXP39KeyIndex[uIndex] - 1;
}
///////////////////////////CTestEXP40///////////////////////////////////
#define EXP40_KEY_NUMBER 20

static short EXP40KeyIndex[EXP40_KEY_NUMBER] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18,
                                                1, 3, 5, 7, 9, 11, 13, 15, 17, 19
                                               };

CTestEXP40::CTestEXP40(UINT uEXPNum) : CTestEXPBWBase(uEXPNum)
{
    m_uMaxKeyNum = EXP40_KEY_NUMBER;
    m_nModuleType = DMT_EXP40_MODULE;
}

CTestEXP40::~CTestEXP40()
{
}

int CTestEXP40::GetKeyIndex(UINT uIndex)
{
    if (uIndex >= m_uMaxKeyNum)
    {
        return -1;
    }

    //todo:
    // 测试模式时，以列为顺序测试按键:即EXP40_KEY_1、EXP40_KEY_3、EXP40_KEY_5...EXP40_KEY_19、EXP40_KEY_2...
    return EXP40KeyIndex[uIndex];
}


///////////////////////////CTestCM69///////////////////////////////////
static BYTE CM69KeyIndex[20] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

CTestCM69::CTestCM69(UINT uEXPNum) : CTestEXPBWBase(uEXPNum)
{
    m_uMaxKeyNum = 20;
    m_nModuleType = DMT_EXPCM69_MODULE;
}

CTestCM69::~CTestCM69()
{
}

int CTestCM69::GetKeyIndex(UINT uIndex)
{
    if (uIndex >= m_uMaxKeyNum)
    {
        return -1;
    }

    return CM69KeyIndex[uIndex] - 1;
}

#ifdef _EXP_COLOR_SUPPORT
///////////////////////////CTestEXP50///////////////////////////////////
#define EXP50_KEY_NUMBER 23
#define EXP50_KEY_PAGE_1 20
#define EXP50_KEY_PAGE_2 21
#define EXP50_KEY_PAGE_3 22

static BYTE EXP50KeyIndex[EXP50_KEY_NUMBER] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18,
                                                1, 3, 5, 7, 9, 11, 13, 15, 17, 19,
                                                EXP50_KEY_PAGE_1, EXP50_KEY_PAGE_2, EXP50_KEY_PAGE_3
                                              };

CTestEXP50::CTestEXP50(UINT uEXPNum) : CTestEXPBWBase(uEXPNum)
{
    m_uMaxKeyNum = EXP50_KEY_NUMBER;
    m_nModuleType = DMT_EXP50_MODULE;
}

CTestEXP50::~CTestEXP50()
{
}

int CTestEXP50::GetKeyIndex(UINT uIndex)
{
    if (uIndex >= m_uMaxKeyNum)
    {
        return -1;
    }

    return EXP50KeyIndex[uIndex];
}

void CTestEXP50::DrawScreenByColor(const EXP_FILL_COLOR & objExpColor, int nIndex/* = 0*/)
{
    exp_ExpFillWithColor(objExpColor, nIndex);
}

void CTestEXP50::DrawScreenByText(const yl::string & strText, int nIndex/* = 0*/)
{
    exp_ExpDrawText(strText.c_str(), nIndex);
}

void CTestEXP50::SetLedStatus(int hop, int row, int col, int led_status)
{
    expusb_led led;
    led.row = row + 1;
    led.col = col + 1;
    led.port = hop;
    led.mask = led_status;
    led.flash_timer = 0;

    expusb_helper_led_set(exp_GetExpHandle(), led);
}

void CTestEXP50::LightOnEXP(int iEXPIndex, LIGHT_Type eType)
{
    CTestEXPBWBase::LightOnEXP(iEXPIndex, eType);

    // 翻页键1，2，3单独处理
    for (int iKey = EXP50_KEY_PAGE_1; iKey <= EXP50_KEY_PAGE_3; ++iKey)
    {
        SetLedStatus(iEXPIndex, 10, iKey - EXP50_KEY_PAGE_1, eType);
    }
}

void CTestEXP50::LightOffByKeyIndex(int iKey)
{
    // 翻页键1，2，3单独处理
    if (iKey >= EXP50_KEY_PAGE_1 && iKey <= EXP50_KEY_PAGE_3)
    {
        int iEXPIndex = GetEXPIndex(iKey);
        SetLedStatus(iEXPIndex, 10, iKey - EXP50_KEY_PAGE_1, LT_OFF);
    }
    else
    {
        CTestEXPBWBase::LightOffByKeyIndex(iKey);
    }
}

void CTestEXP50::DrawEXPLCD(int iStep)
{
    switch (iStep)
    {
    case 0:
        {
            // 全红
            EXP_FILL_COLOR stuRed;
            stuRed.nRed = 255;
            stuRed.nGreen = 0;
            stuRed.nBlue = 0;
            stuRed.bGradient = false;
            exp_ExpFillWithColor(stuRed);
        }
        break;
    case 1:
        {
            // 全绿
            EXP_FILL_COLOR stuGreen;
            stuGreen.nRed = 0;
            stuGreen.nGreen = 255;
            stuGreen.nBlue = 0;
            stuGreen.bGradient = false;
            exp_ExpFillWithColor(stuGreen);
        }
        break;
    case 2:
        {
            // 全蓝
            EXP_FILL_COLOR stuBlue;
            stuBlue.nRed = 0;
            stuBlue.nGreen = 0;
            stuBlue.nBlue = 255;
            stuBlue.bGradient = false;
            exp_ExpFillWithColor(stuBlue);
        }
        break;
    case 3:
        {
            // 黑白棋盘格
            exp_ExpFillWithPicture(PIC_TESTMODE_EXPGRID);
        }
        break;
    case 4:
        {
            // 80度灰
            exp_ExpFillWithPicture(PIC_TESTMODE_EXPGRAY80);
        }
        break;
    case 5:
        {
            // 50度灰
            exp_ExpFillWithPicture(PIC_TESTMODE_EXPGRAY50);
        }
        break;
    case 6:
        {
            // 30度灰
            exp_ExpFillWithPicture(PIC_TESTMODE_EXPGRAY30);
        }
        break;
    case 7:
        {
            exp_ExpFillWithPicture(PIC_TESTMODE_EXPFLICK);
        }
        break;
    case 8:
        {
            // 灰阶显示
            EXP_FILL_COLOR stuBlackToWhite;
            stuBlackToWhite.nRed = 0;
            stuBlackToWhite.nGreen = 0;
            stuBlackToWhite.nBlue = 0;
            stuBlackToWhite.bGradient = true;
            exp_ExpFillWithColor(stuBlackToWhite);
        }
        break;
    case 9:
        {
            // 全白
            EXP_FILL_COLOR stuWhite;
            stuWhite.nRed = 255;
            stuWhite.nGreen = 255;
            stuWhite.nBlue = 255;
            stuWhite.bGradient = false;
            exp_ExpFillWithColor(stuWhite);
        }
        break;
    default:
        break;
    }
}

void CTestEXP50::ExitEXPTest()
{
    DrawEXPLCD(9);
}
#endif
