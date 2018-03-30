#include "teststatusicon.h"
#include <QTimer>
#include "mainwnd/mainwnd.h"
#include "titlebar.h"
#include "dsklog/log.h"

#define TEST_STATUS_ICON_INTERVAL (5 * 1000)
#define STATUS_ICON_MAX_SIZE       5

TTestStatusIcon & GetTestStatusIcon()
{
    static TTestStatusIcon objTestStatusIcon;
    return objTestStatusIcon;
}

TTestStatusIcon::TTestStatusIcon()
    : m_pTimer(NULL)
{
    m_pTimer = new QTimer(this);
    if (NULL != m_pTimer)
    {
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
    }
}

TTestStatusIcon::~TTestStatusIcon()
{

}

void TTestStatusIcon::StatrTest()
{
    MAINWND_INFO("TTestStatusIcon::StatrTest");
    if (NULL != m_pTimer)
    {
        m_pTimer->start(TEST_STATUS_ICON_INTERVAL);
    }
}

void TTestStatusIcon::StopTest()
{
    if (NULL != m_pTimer)
    {
        m_pTimer->stop();
    }
}

void TTestStatusIcon::OnTimeOut()
{
    MAINWND_INFO("TTestStatusIcon::OnTimeOut");
    if (m_listStatusItem.size() < STATUS_ICON_MAX_SIZE + 1)
    {
        IdleStatusItem objItem;
        objItem.m_nId = PS_STATE_MISSCALL;
        objItem.m_strHint = m_strText.c_str();
        m_listStatusItem.push_back(objItem);
    }
    else
    {
        m_strText += "9";
        m_listStatusItem.clear();
    }

    QWidget * pWgt = _MainWND.GetTitle();
    if (NULL != pWgt
            && pWgt->inherits("CTitleBar"))
    {
        CTitleBar * pTitleBar = static_cast<CTitleBar *>(pWgt);
        MAINWND_INFO("TTestStatusIcon::RefreshStatusIcon");
        pTitleBar->RefreshStatusIcon(m_listStatusItem);
    }
}
