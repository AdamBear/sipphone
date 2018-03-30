#include "cdlgautoanswertimer.h"
#include "account/include/modaccount.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include "keymap.h"
#include "baseui/fontcolor.h"
#include "setting/src/autoanswercontroller.h"
#include "uikernel/qcheckbox.h"
#include "setting/include/authorityiddefine.h"
#include "uikernel/qcomboboxutility.h"

#ifdef IF_FEATURE_AUTO_ANSWER_TIMER

CDlgAutoAnswerTimer::CDlgAutoAnswerTimer(QWidget * parent /*= 0*/)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxTimer(NULL)
{
    InitData();
}

void CDlgAutoAnswerTimer::InitData()
{
    QWidgetPair qPair;
    QString strLab;

    m_pCmbBoxTimer = new QComboBox(this);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxTimer, Qt::black);
    for (int iIndex = 1; iIndex <= 4; ++iIndex)
    {
        yl::string strIndex = commonAPI_FormatString("%d ", iIndex);
        QString strDsp = toQString(strIndex) + LANG_TRANSLATE(TRID_SECOND);
        m_pCmbBoxTimer->addItem(strDsp, iIndex);
    }

    strLab = LANG_TRANSLATE(TRID_AUTO_ANSWER_TIMER);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxTimer;
    qPair.strAuthorId = kszAutoAnswerTimer;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    m_pCmbBoxTimer->installEventFilter(this);
    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

bool CDlgAutoAnswerTimer::LoadPageData()
{
    this->setFocus();

    int iTimer = g_pAutoAnswerController->GetAutoAnswerTimer();
    m_pCmbBoxTimer->setCurrentIndex(m_pCmbBoxTimer->findData(iTimer, Qt::UserRole));

    return true;
}

bool CDlgAutoAnswerTimer::IsStatusChanged()
{
    int iTimer = g_pAutoAnswerController->GetAutoAnswerTimer();
    int iPageTimer = m_pCmbBoxTimer->itemData(m_pCmbBoxTimer->currentIndex(),
                     Qt::UserRole).value<int>();

    return iTimer != iPageTimer;
}

bool CDlgAutoAnswerTimer::SavePageData()
{
    int iPageTimer = m_pCmbBoxTimer->itemData(m_pCmbBoxTimer->currentIndex(),
                     Qt::UserRole).value<int>();
    g_pAutoAnswerController->SetAutoAnswerTimer(iPageTimer);
    return true;
}

CDlgAutoAnswerTimer::~CDlgAutoAnswerTimer()
{
}
#endif
