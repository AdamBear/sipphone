#include "dlgvoicemaillist.h"

#if IF_FEATURE_BROADSOFT_VOICEMAIL

#include "xwindow/xWindow.h"
#include "baseui/include/baseuicommon.h"
#include "basesettinguidelegate.h"
#include "settingui/include/pageiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"
#include "edk/include/modedk.h"
#include "edkui/include/modedkui.h"

CBaseDialog * CDlgVoiceMailList::CreateInstance()
{
    CDlgVoiceMailList * pDlg = new CDlgVoiceMailList;

    if (NULL != pDlg)
    {
        pDlg->loadContent("setting/dlgmessageview.xml");
    }

    return pDlg;
}

CDlgVoiceMailList::CDlgVoiceMailList()
{
}

CDlgVoiceMailList::~CDlgVoiceMailList()
{
}

void CDlgVoiceMailList::loadChildrenElements(xml_node & node)
{
    CDlgSettingUI::loadChildrenElements(node);

    m_VoiceMailTitle.SetTitleView(getViewById("BoxGuideTitle"));
}

void CDlgVoiceMailList::RefreshTitle(const yl::string & strTitle)
{
    yl::string strRealTitle = strTitle;
    if (Edk_IsEnable() && EDK_IsTipModeEnable())
    {
        strRealTitle = EdkUI_GetMenuIDByPageData(kszPageIDVoiceMailList, "", -1);
    }
    m_VoiceMailTitle.SetTitle(strRealTitle);

    yl::string strPageInfo = "";
    m_VoiceMailTitle.SetGuideInfo(strPageInfo);

    if (NULL != m_pSettingUIDelegate)
    {
        int nFocusIndex = m_pSettingUIDelegate->GetFocusIndex();
        int nTotalCount = m_pSettingUIDelegate->GetTotalDataCount();
        if (nFocusIndex >= 0 && nTotalCount > 0)
        {
            ++nFocusIndex;
        }

        yl::string strPageInfo = commonAPI_FormatString("%d/%d", nFocusIndex, nTotalCount);
        m_VoiceMailTitle.SetPageInfo(strPageInfo);
    }
}

bool CDlgVoiceMailList::onActiveEvent(bool bActive)
{
    if (bActive && NULL != m_pSettingUIDelegate && m_pSettingUIDelegate->IsRefreshDataWhenTopDelegate())
    {
        m_pSettingUIDelegate->SetRefreshDataWhenTopDelegate(false);
        RefreshUI(true);
        return true;
    }

    return CDlgSettingUI::onActiveEvent(bActive);
}

#endif
