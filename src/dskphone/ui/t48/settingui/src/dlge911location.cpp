#if IF_FEATURE_GENBAND_E911
#include "dlge911location.h"
#include "uikernel/languagehelper.h"
#include "include/translateiddefine.h"
#include "e911locationcontroller.h"
#include "modsettingui.h"
#include "baseui/t4xpicpath.h"
#include "e911address/include/mode911address.h"

#define ACTION_RESET "Reset"
namespace
{
const QRect RECT_DLG_LOCATION(15, 55, 501, 218);
}


CDlgE911Location::CDlgE911Location(QWidget *parent)
    : CDlgBaseSubPage(parent)
{
    InitData();
}

CDlgE911Location::~CDlgE911Location()
{

}

void CDlgE911Location::InitData()
{
    UpdateText();

    this->installEventFilter(this);

    //m_pE911Location->setText("12354664879");
}

void CDlgE911Location::BeforeSetSubPage(void* pData)
{
    SETTINGUI_INFO("CDlgE911Location::BeforeSetSubPage");
    SetWindowTitle(LANG_TRANSLATE(TRID_E911_LOCATION));
}

void CDlgE911Location::Uninit()
{
}

void CDlgE911Location::showEvent(QShowEvent* e)
{

}

bool CDlgE911Location::LoadPageData()
{
    SETTINGUI_INFO("CDlgE911Location::LoadPageData");
    this->setFocus();

    return true;
}

bool CDlgE911Location::GetSoftkeyData(CArraySoftKeyBarData& objSoftData)
{
    SETTINGUI_INFO("CDlgE911Location::GetSoftkeyData");

    if (objSoftData.size() < SOFTKEY_NUM)
    {
        return true;
    }

    objSoftData[0].m_strSoftkeyAction = ACTION_RESET;
    objSoftData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_RESET);
    objSoftData[0].m_strIconPath = PIC_SOFTKEY_RESET;

    objSoftData[1].m_strSoftkeyAction = "";
    objSoftData[1].m_strSoftkeyTitle = "";
    objSoftData[2].m_strSoftkeyAction = "";
    objSoftData[2].m_strSoftkeyTitle = "";
    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle = "";


    return true;
}

void CDlgE911Location::UpdateText()
{
    // 清空之前数据
    ClearAllContent();
    //m_vecWidgetPair.clear();
    yl::string strText = e911_GetCurrentLocation();

    if (!strText.empty())
    {
        QWidgetPair qPair;
        qPair.first = strText.c_str();
        qPair.second = NULL;
        qPair.eType = SETTING_ITEM_NORMAL;
        qPair.bAddIndex = false;
        m_vecWidgetPair.push_back(qPair);

        AddWidget(m_vecWidgetPair, false, false);
    }
//  yl::string strText = e911_GetCurrentLocation();
//  m_pE911Location->setText(strText.c_str());

    UpdateWnd();
}

bool CDlgE911Location::OnAction(const QString& strAction)
{
    SETTINGUI_INFO("CDlgE911Location::OnAction, strAction = %s", strAction.toUtf8().data());

    if (ACTION_RESET == strAction)
    {
        g_E911Controller.ResetE911Location();
        return true;
    }

    return false;
}

#ifdef _T48
bool CDlgE911Location::PorcessBackBtnClick()
{
    return false;
}

bool CDlgE911Location::PorcessHomeBtnClick()
{
    emit BtnBackClick();
    UIManager_ReturnToIdle(true);
    return true;
}

bool CDlgE911Location::ProcessFunctionkey(int nKeyCode, bool bPress)
{
    return true;
}

bool CDlgE911Location::IsRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle/* = RETURN_IDLE_TYPE_NORMAL*/)
{
    return false;
}

void CDlgE911Location::OnReShow()
{
    if (m_bRefreshOnReshow)
    {
        UpdateWnd();
        m_bRefreshOnReshow = false;
    }
    else
    {
        CDlgBaseSubPage::OnReShow();
    }
}
#endif
#endif