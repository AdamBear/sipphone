#include "cdlggetbalance.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "account/include/getbalance.h"
#include "uicommon/qtcommon/qmisc.h"

#ifdef IF_FEATURE_GETBALANCE
CDlgGetBalance::CDlgGetBalance(QWidget * parent) : CDlgBaseSubPage(parent)
{
    InitPageData();
    this->installEventFilter(this);
}

CDlgGetBalance::~CDlgGetBalance()
{
    StopTimer();
}

void CDlgGetBalance::InitPageData()
{
    m_TimerAutoQuit = new QTimer(this);
    connect(m_TimerAutoQuit, SIGNAL(timeout()), this, SLOT(OnTimer()));

    m_pLabelAccount = new QLabel(this);
    m_pLabelBalance = new QLabel(this);

    SetWgtFontColor(m_pLabelAccount, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabelBalance, FONT_TYPE_TEXT);

    QWidgetPair qPair;
    qPair.first = LANG_TRANSLATE(TRID_ACCOUNT);
    qPair.second = m_pLabelAccount;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_BALANCE);
    qPair.second = m_pLabelBalance;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair, false);
}

bool CDlgGetBalance::LoadPageData()
{
    setFocus();
    // 5秒超时退出
    StartTimer(BALANCE_MSG_BOX_SHOW_NORMAL);

    Balance_Info_t info;
    _AccBalanceManager.GetBalanceInfo(info);

    m_strTitle = LANG_TRANSLATE(info.szName);

    QString strAccount = toQString(info.szAccount);
    m_pLabelAccount->setText(strAccount);

    QString strBalance = toQString(info.szBalance);
    m_pLabelBalance->setText(strBalance);

    return true;
}

bool CDlgGetBalance::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            StartTimer();
        }
        break;
    case QEvent::Destroy:
    case QEvent::Hide:
        {
            StopTimer();
        }
        break;
    case QEvent::Show:
        {
            StartTimer();
            this->setFocus();
        }
        break;
    default:
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgGetBalance::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    // 第1个softkey显示为Exit，默认只有一个Softkey
    objSoftData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_EXIT);

    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle = "";

    return true;
}

void CDlgGetBalance::StopTimer()
{
    if (m_TimerAutoQuit->isActive())
    {
        m_TimerAutoQuit->stop();
    }
}

void CDlgGetBalance::StartTimer(int timer/* = 0*/)
{
    StopTimer();
    m_TimerAutoQuit->start(timer > 0 ? timer : BALANCE_MSG_BOX_SHOW_NORMAL);
}

void CDlgGetBalance::OnTimer()
{
    StopTimer();

    emit BtnBackClick();
}

#endif
