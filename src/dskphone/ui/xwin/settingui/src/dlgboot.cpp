#include "dlgboot.h"
#ifdef IF_SETTING_WIZARD
#include "xwindow/xWindow.h"
#include "settinguiwizardtitle.h"
#include "baseui/include/baseuicommon.h"
#include "basesettinguidelegate.h"
#include "settinguiwizardmanager.h"
#include "edk/include/modedk.h"
#include "edkui/include/modedkui.h"
#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"

CBaseDialog * CDlgBoot::CreateInstance()
{
    CDlgBoot * pDlg = new CDlgBoot;

    if (NULL != pDlg)
    {
        pDlg->loadContent("setting/dlgboot.xml");
    }

    return pDlg;
}

CDlgBoot::CDlgBoot()
{
}

CDlgBoot::~CDlgBoot()
{
}

void CDlgBoot::loadChildrenElements(xml_node & node)
{
    CDlgSettingUI::loadChildrenElements(node);

    m_GuideTitle.SetTitleView(getViewById("BoxGuideTitle"));
}

void CDlgBoot::RefreshTitle(const yl::string & strTitle)
{
    yl::string strRealTitle = strTitle;
    if (Edk_IsEnable() && EDK_IsTipModeEnable() && NULL != m_pSettingUIDelegate)
    {
        yl::string strPageID = m_pSettingUIDelegate->GetPageID();
        yl::string strNextPageID = m_pSettingUIDelegate->GetNextPageID();
        int nID =  m_pSettingUIDelegate->GetPageLineID();
        strRealTitle = EdkUI_GetMenuIDByPageData(strPageID, strNextPageID, nID);
    }

    m_GuideTitle.SetTitle(strRealTitle);

    yl::string strPageInfo = "";
    if (NULL != g_pSettingUIWizardManager && NULL != m_pSettingUIDelegate)
    {
        int nIndex = g_pSettingUIWizardManager->GetCurrentIndex(m_pSettingUIDelegate->GetPageID());
        if (-1 != nIndex)
        {
            int nTotalNum = g_pSettingUIWizardManager->GetTotalGuidePageCount();
            strPageInfo = commonAPI_FormatString("%d/%d", ++nIndex, nTotalNum);
        }
    }

    m_GuideTitle.SetPageInfo(strPageInfo);

    m_GuideTitle.SetGuideInfo(_LANG(TRID_WIZARD_TITLE));
}

#endif //IF_SETTING_WIZARD
