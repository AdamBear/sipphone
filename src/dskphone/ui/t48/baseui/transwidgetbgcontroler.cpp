#include "transwidgetbgcontroler.h"
#include "t4xpicpath.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

static const char szTransTypeLowTrans[] = "0%";
static const char szTransType20Percent[] = "20%";
static const char szTransType40Percent[] = "40%";
static const char szTransType60Percent[] = "60%";
static const char szTransType80Percent[] = "80%";
static const char szTransTypeHeightTrans[] = "100%";


IMPLEMENT_GETINSTANCE(CTransWidgetBgControler)

yl::string CTransWidgetBgControler::GetTransValueByType(int nType)
{
    switch (nType)
    {
    case TRANSPARENT_TYPE_LOW_TRANS:
        {
            return szTransTypeLowTrans;
        }
    case TRANSPARENT_TYPE_TWENTY_PERCENT:
        {
            return szTransType20Percent;
        }
    case TRANSPARENT_TYPE_FORTY_PERCENT:
        {
            return szTransType40Percent;
        }
    case TRANSPARENT_TYPE_SIXTY_PERCENT:
        {
            return szTransType60Percent;
        }
    case TRANSPARENT_TYPE_EIGHTY_PERCENT:
        {
            return szTransType80Percent;
        }
    case TRANSPARENT_TYPE_HEIGHT_TRANS:
        {
            return szTransTypeHeightTrans;
        }
    default:
        break;
    }

    return szTransTypeHeightTrans;
}

CTransWidgetBgControler::CTransWidgetBgControler()
    : m_bCurDlgUseTransparency(false)
{
    UpdateTransType();
}

CTransWidgetBgControler::~CTransWidgetBgControler()
{
}

void CTransWidgetBgControler::UpdateTransType()
{
    yl::string strTransLevel = configParser_GetConfigString(kszTransparencyType);

    m_nTransType = TRANSPARENT_TYPE_HEIGHT_TRANS;

    if (szTransTypeLowTrans == strTransLevel)
    {
        m_nTransType = TRANSPARENT_TYPE_LOW_TRANS;
    }
    else if (szTransType20Percent == strTransLevel)
    {
        m_nTransType = TRANSPARENT_TYPE_TWENTY_PERCENT;
    }
    else if (szTransType40Percent == strTransLevel)
    {
        m_nTransType = TRANSPARENT_TYPE_FORTY_PERCENT;
    }
    else if (szTransType60Percent == strTransLevel)
    {
        m_nTransType = TRANSPARENT_TYPE_SIXTY_PERCENT;
    }
    else if (szTransType80Percent == strTransLevel)
    {
        m_nTransType = TRANSPARENT_TYPE_EIGHTY_PERCENT;
    }

    ChangeAllTransWidgetBg();
}

void CTransWidgetBgControler::ChangeAllTransWidgetBg()
{
    switch (m_nTransType)
    {
    case TRANSPARENT_TYPE_LOW_TRANS:
        {
            GetLowTransPicture();
        }
        break;
    case TRANSPARENT_TYPE_TWENTY_PERCENT:
        {
            GetTwentyPercentTransPicture();
        }
        break;
    case TRANSPARENT_TYPE_FORTY_PERCENT:
        {
            GetFortyPercentTransPicture();
        }
        break;
    case TRANSPARENT_TYPE_SIXTY_PERCENT:
        {
            GetSixtyPercentTransPicture();
        }
        break;
    case TRANSPARENT_TYPE_EIGHTY_PERCENT:
        {
            GetEightyPercentTransPicture();
        }
        break;
    default:
        {
            GetHeightTransPicture();
        }
        break;
    }
}

void CTransWidgetBgControler::SetCurDlgUseTransparency(bool bUseTrans)
{
    if (m_bCurDlgUseTransparency == bUseTrans)
    {
        return;
    }

    m_bCurDlgUseTransparency = bUseTrans;

    if (!m_bCurDlgUseTransparency)
    {
        // 使用默认高透图片
        GetHeightTransPicture();
        return;
    }

    ChangeAllTransWidgetBg();
}

bool CTransWidgetBgControler::IsCurDlgUseTransparency()
{
    return m_bCurDlgUseTransparency;
}

int CTransWidgetBgControler::GetTransparencyType()
{
    return m_nTransType;
}

yl::string CTransWidgetBgControler::GetTitleBarBg()
{
    return m_strTitleBarBg;
}

yl::string CTransWidgetBgControler::GetNormalDsskeyBg()
{
    return m_strDssBgNormal;
}

yl::string CTransWidgetBgControler::GetEnableDsskeyBg()
{
    return m_strDssBgEnable;
}

yl::string CTransWidgetBgControler::GetDisableDsskeyBg()
{
    return m_strDssBgDisable;
}

yl::string CTransWidgetBgControler::GetHeightlightDsskeyBg()
{
    return m_strDssBgHeightlight;
}

yl::string CTransWidgetBgControler::GetUsingDsskeyBg()
{
    return m_strDssBgUsing;
}

yl::string CTransWidgetBgControler::GetGreenDsskeyBg()
{
    return m_strDssBgGreen;
}

yl::string CTransWidgetBgControler::GetRedDsskeyBg()
{
    return m_strDssBgRed;
}

yl::string CTransWidgetBgControler::GetOrangeDsskeyBg()
{
    return m_strDssBgOrange;
}

void CTransWidgetBgControler::GetLowTransPicture()
{
    m_strTitleBarBg = PIC_THEME1_LOW_TRANS_TITLEBIG;
    m_strDssBgNormal = PIC_DSS_BTN_LOW_TRANS_NORMAL;
    m_strDssBgEnable = PIC_DSS_BTN_LOW_TRANS_ENABLE;
    m_strDssBgDisable = PIC_DSS_BTN_LOW_TRANS_NORMAL;
    m_strDssBgHeightlight = PIC_DSS_BTN_LOW_TRANS_HIGHLIGHT;
    m_strDssBgUsing = PIC_DSS_BTN_LOW_TRANS_USING;
    m_strDssBgGreen = PIC_DSS_SET_LOW_TRANS_GREEN;
    m_strDssBgRed = PIC_DSS_SET_LOW_TRANS_RED;
    m_strDssBgOrange = PIC_DSS_SET_LOW_TRANS_ORANGE;
}

void CTransWidgetBgControler::GetTwentyPercentTransPicture()
{
    m_strTitleBarBg = PIC_THEME1_T_PERCENT_TRANS_TITLEBIG;
    m_strDssBgNormal = PIC_DSS_BTN_T_PERCENT_TRANS_NORMAL;
    m_strDssBgEnable = PIC_DSS_BTN_T_PERCENT_TRANS_ENABLE;
    m_strDssBgDisable = PIC_DSS_BTN_T_PERCENT_TRANS_NORMAL;
    m_strDssBgHeightlight = PIC_DSS_BTN_T_PERCENT_TRANS_HIGHLIGHT;
    m_strDssBgUsing = PIC_DSS_BTN_T_PERCENT_TRANS_USING;
    m_strDssBgGreen = PIC_DSS_SET_T_PERCENT_TRANS_GREEN;
    m_strDssBgRed = PIC_DSS_SET_T_PERCENT_TRANS_RED;
    m_strDssBgOrange = PIC_DSS_SET_T_PERCENT_TRANS_ORANGE;
}

void CTransWidgetBgControler::GetFortyPercentTransPicture()
{
    m_strTitleBarBg = PIC_THEME1_F_PERCENT_TRANS_TITLEBIG;
    m_strDssBgNormal = PIC_DSS_BTN_F_PERCENT_TRANS_NORMAL;
    m_strDssBgEnable = PIC_DSS_BTN_F_PERCENT_TRANS_ENABLE;
    m_strDssBgDisable = PIC_DSS_BTN_F_PERCENT_TRANS_NORMAL;
    m_strDssBgHeightlight = PIC_DSS_BTN_F_PERCENT_TRANS_HIGHLIGHT;
    m_strDssBgUsing = PIC_DSS_BTN_F_PERCENT_TRANS_USING;
    m_strDssBgGreen = PIC_DSS_SET_F_PERCENT_TRANS_GREEN;
    m_strDssBgRed = PIC_DSS_SET_F_PERCENT_TRANS_RED;
    m_strDssBgOrange = PIC_DSS_SET_F_PERCENT_TRANS_ORANGE;
}

void CTransWidgetBgControler::GetSixtyPercentTransPicture()
{
    m_strTitleBarBg = PIC_THEME1_S_PERCENT_TRANS_TITLEBIG;
    m_strDssBgNormal = PIC_DSS_BTN_S_PERCENT_TRANS_NORMAL;
    m_strDssBgEnable = PIC_DSS_BTN_S_PERCENT_TRANS_ENABLE;
    m_strDssBgDisable = PIC_DSS_BTN_S_PERCENT_TRANS_NORMAL;
    m_strDssBgHeightlight = PIC_DSS_BTN_S_PERCENT_TRANS_HIGHLIGHT;
    m_strDssBgUsing = PIC_DSS_BTN_S_PERCENT_TRANS_USING;
    m_strDssBgGreen = PIC_DSS_SET_S_PERCENT_TRANS_GREEN;
    m_strDssBgRed = PIC_DSS_SET_S_PERCENT_TRANS_RED;
    m_strDssBgOrange = PIC_DSS_SET_S_PERCENT_TRANS_ORANGE;
}

void CTransWidgetBgControler::GetEightyPercentTransPicture()
{
    m_strTitleBarBg = PIC_THEME1_E_PERCENT_TRANS_TITLEBIG;
    m_strDssBgNormal = PIC_DSS_BTN_E_PERCENT_TRANS_NORMAL;
    m_strDssBgEnable = PIC_DSS_BTN_E_PERCENT_TRANS_ENABLE;
    m_strDssBgDisable = PIC_DSS_BTN_E_PERCENT_TRANS_NORMAL;
    m_strDssBgHeightlight = PIC_DSS_BTN_E_PERCENT_TRANS_HIGHLIGHT;
    m_strDssBgUsing = PIC_DSS_BTN_E_PERCENT_TRANS_USING;
    m_strDssBgGreen = PIC_DSS_SET_E_PERCENT_TRANS_GREEN;
    m_strDssBgRed = PIC_DSS_SET_E_PERCENT_TRANS_RED;
    m_strDssBgOrange = PIC_DSS_SET_E_PERCENT_TRANS_ORANGE;
}

void CTransWidgetBgControler::GetHeightTransPicture()
{
    m_strTitleBarBg = "";
    m_strDssBgNormal = PIC_DSS_BTN_NORMAL;
    m_strDssBgEnable = PIC_DSS_BTN_ENABLE;
    m_strDssBgDisable = PIC_DSS_BTN_NORMAL;
    m_strDssBgHeightlight = PIC_DSS_BTN_HIGHLIGHT;
    m_strDssBgUsing = PIC_DSS_BTN_USING;
    m_strDssBgGreen = PIC_DSS_SET_GREEN;
    m_strDssBgRed = PIC_DSS_SET_RED;
    m_strDssBgOrange = PIC_DSS_SET_ORANGE;
}
