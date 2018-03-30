#include "dlgpingandtraceout.h"
#include "xwindow/xWindow.h"
#include "settinguiscrollbypagedelegatebase.h"
#include "settingui/include/pageiddefine.h"

namespace
{
#define SCROLL_PAGE_COUNT 1
}

CBaseDialog * CDlgPingAndTraceOut::CreateInstance()
{
    CDlgPingAndTraceOut * pDlg = new CDlgPingAndTraceOut;

    if (NULL != pDlg)
    {
        pDlg->loadContent("setting/dlgpingandtraceresult.xml");
    }

    return pDlg;
}

CDlgPingAndTraceOut::CDlgPingAndTraceOut()
    : m_pScroll(NULL)
{
}

CDlgPingAndTraceOut::~CDlgPingAndTraceOut()
{
}

void CDlgPingAndTraceOut::loadChildrenElements(xml_node & node)
{
    CDlgSettingUI::loadChildrenElements(node);

    m_pScroll = static_cast<xScrollBar *>(getViewById("scroll"));
}

void CDlgPingAndTraceOut::RefreshUI(bool bReLoadData /*= false*/)
{
    CDlgSettingUI::RefreshUI(bReLoadData);

    if (NULL == m_pScroll || !IsScrollBypageDelegate())
    {
        return;
    }

    CSettinguiScrollByPageDelegateBase * pDelegate = (CSettinguiScrollByPageDelegateBase *)
            m_pSettingUIDelegate;
    if (NULL == pDelegate)
    {
        return ;
    }

    int nTotal = pDelegate->GetTotalPage();
    if (nTotal <= 1)
    {
        m_pScroll->hide();
        return;
    }
    else
    {
        m_pScroll->show();
    }

    m_pScroll->SetScrollInfo(SCROLL_PAGE_COUNT, pDelegate->GetTotalPage(), pDelegate->getCurrentPage());
}

bool CDlgPingAndTraceOut::IsScrollBypageDelegate()
{
    if (NULL != m_pSettingUIDelegate)
    {
        yl::string strPageID = m_pSettingUIDelegate->GetPageID();
        if (kszPageIDPingResult == strPageID
                || kszPageIDTraceResult == strPageID
                || kszPageIDStatusRTP == strPageID)
        {
            return true;
        }
    }

    return false;
}
