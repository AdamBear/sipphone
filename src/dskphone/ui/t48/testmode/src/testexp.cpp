#include "testmode_common.h"
#include "testexp.h"
#include "testphonet4x.h"
#include "exp/include/modexp.h"
#include "dsskey/include/moddsskey.h"
#include "exp/include/modexp.h"

///////////////////////////CTestEXP///////////////////////////////////

CTestEXP::CTestEXP(UINT uEXPNum, EXP_TYPE    eEXPType, UINT ulightNum, UINT uMaxKeyNum)
{

    m_uEXPNum = uEXPNum;
    m_eEXPType = eEXPType ;
    m_ulightNum = ulightNum;
    m_uMaxKeyNum = uMaxKeyNum;
    InitDsskey_Module_Type();
}
void CTestEXP::InitDsskey_Module_Type()
{
    switch (m_eEXPType)
    {
    case ET_EXP38:
        {
            m_eDssKeyModuleType = DMT_EXP38_MODULE;
            m_isNeedTestEXPLCD = false;
        }
        break;
    case ET_EXP39:
        {
            m_eDssKeyModuleType = DMT_EXP39_MODULE;
            m_isNeedTestEXPLCD = true;
        }
        break;
    case ET_EXP40:
        {
            m_eDssKeyModuleType = DMT_EXP40_MODULE;
            m_isNeedTestEXPLCD = true;
        }
        break;
    default:
        m_eDssKeyModuleType = DMT_NON;
        m_isNeedTestEXPLCD = false;
        break;
    }
}
CTestEXP::~CTestEXP()
{

}

void CTestEXP::LightOnAllEXP()
{
    LightEXP(m_uEXPNum, (LIGHT_Type)EXP_LED_ORANGE);
}

void CTestEXP::LightOffAllEXP()
{
    LightEXP(m_uEXPNum, (LIGHT_Type)EXP_LED_OFF);
}

void CTestEXP::LightEXP(int iEXPNum, LIGHT_Type iLightType)
{
    //todo:
    int DsskeyID;
    for (int iEXPIndex = 0; iEXPIndex < iEXPNum; iEXPIndex++)
    {
        for (int i = 0 ; i <= m_ulightNum - 1 ; i++)
        {
            if (DMT_NON != m_eDssKeyModuleType)
            {
                DsskeyID = dsskey_GetDsskeyID(m_eDssKeyModuleType, iEXPIndex, i);
				exp_SetExpLEDStatus(DsskeyID, iLightType);
            }
        }
    }
}

void CTestEXP::LightOffByKeyIndex(int iKey)//关闭某个灯
{
    //todo:
    int iEXPIndex = GetEXPIndex(iKey);//第几个扩展台
    int iEXPkeyindex = GetEXPKeyIndex(iKey);//扩展台上的第几个按键
    int DsskeyID;
    if (iEXPIndex < m_uEXPNum)
    {
        DsskeyID = dsskey_GetDsskeyID(m_eDssKeyModuleType, iEXPIndex, iEXPkeyindex);
		exp_SetExpLEDStatus(DsskeyID, LT_OFF);
    }
}
void CTestEXP::LightOnByKeyIndex(int iKey, LIGHT_Type iLightType) //关闭某个灯
{
    //todo:
    int iEXPIndex = GetEXPIndex(iKey);//第几个扩展台
    int iEXPkeyindex = GetEXPKeyIndex(iKey);//扩展台上的第几个按键
    int DsskeyID;
    if (iEXPIndex < m_uEXPNum)
    {
        DsskeyID = dsskey_GetDsskeyID(m_eDssKeyModuleType, iEXPIndex, iEXPkeyindex);
		exp_SetExpLEDStatus(DsskeyID, iLightType);
    }
}

void CTestEXP::LightOnFirstEXP(LIGHT_STEP eStep)
{
    LIGHT_Type eType = (LIGHT_Type)EXP_LED_OFF;
    switch (eStep)
    {
    case LS_GREEN:
        eType = (LIGHT_Type)EXP_LED_GREEN;
        break;
    case LS_RED:
        eType = (LIGHT_Type)EXP_LED_RED;
        break;
    case LS_ORANGE:
        eType = (LIGHT_Type)EXP_LED_ORANGE;
        break;
    default:
        break;
    }

    LightEXP(1, eType);
    //点亮第二个扩展台上的灯
    int     DsskeyID = dsskey_GetDsskeyID(m_eDssKeyModuleType, 1, 0);
	exp_SetExpLEDStatus(DsskeyID, eType);
}

int CTestEXP::GetKeyIndex(UINT uIndex)
{
    if (uIndex >= m_uMaxKeyNum)
    {
        return -1;
    }

    //todo:
    // 测试模式时，
    //EXP40以列为顺序测试按键:即EXP40_KEY_1、EXP40_KEY_3、EXP40_KEY_5...EXP40_KEY_19、EXP40_KEY_2...
    //EXP39和EXP40类似 EXP38待确定。
    if (DMT_EXP38_MODULE == m_eDssKeyModuleType)
    {
        return EXP38KeyIndex[uIndex];
    }
    else if (DMT_EXP39_MODULE == m_eDssKeyModuleType)
    {
        return EXP39KeyIndex[uIndex];
    }
    else if (DMT_EXP40_MODULE == m_eDssKeyModuleType)
    {
        return  EXP40KeyIndex[uIndex];
    }
    else
    {
        return -1;
    }

}
chString CTestEXP::GetEXPKeyString(UINT uIndex)
{
    if (uIndex > m_uMaxKeyNum)
    {
        return "";
    }

    chString strRetVal = "";
    //    chString strTmp = "";
    char strTmp [20];
    memset(strTmp, 0, 20);
    for (int i = 0; i < (int)uIndex; i++)
    {
        //  strTmp.Format("%d ", i+1);
        sprintf(strTmp, "%d ", i + 1);
        strRetVal += strTmp;
    }

    return strRetVal;
}
void CTestEXP::DrawEXPLCD(int iStep)
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
            ShowEXPLogo(EXP_TM_PIC0);
        }
        break;
    case 4:
        {
            // 其他图片
            ShowEXPLogo(EXP_TM_PIC1);
        }
        break;
    case 5:
        {
            // 其他图片
            ShowEXPLogo(EXP_TM_PIC2);
        }
        break;
    case 6:
        {
            // 其他图片
            ShowEXPLogo(EXP_TM_PIC3);
        }
        break;
    case 7:
        {
            // 其他图片
            ShowEXPLogo(EXP_TM_PIC0);
        }
        break;
    default:
        break;
    }
}
void CTestEXP::ShowEXPGray(BYTE byIndex)
{
    exp_helper_set_lcd_testmode(g_pTestPhone->m_iEXPHandle, 0, byIndex);
}

void CTestEXP::ShowEXPLogo(BYTE byIndex)
{
    exp_helper_set_lcd_testmode(g_pTestPhone->m_iEXPHandle, 0, byIndex);
}

