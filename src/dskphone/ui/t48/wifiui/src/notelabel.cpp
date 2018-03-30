#include "notelabel.h"
#include <QTimer>
#include "ylstring.h"

namespace
{
#define NOTE_DISAPPEAR_TIME         2000
#define NOTE_ANIMATION_TIME         500

#ifdef _T49
#define DEFAULT_NOTE_TEXT_SIZE      23
#else
#define DEFAULT_NOTE_TEXT_SIZE      16
#endif

#ifdef _T49
// #66767B
#define DEFAULT_INFO_TEXT_COLOR QColor(102, 118, 123)
#endif
}

NoteLabel::NoteLabel(QWidget * parent /*= NULL*/)
    : QLabel(parent)
    , m_pTimer(NULL)
    , m_strDis("")
    , m_eType(INT_NONE)
    , m_iCount(0)
{
    m_pTimer = new QTimer(this);
    if (NULL == m_pTimer)
    {
        return;
    }

    QFont fontLable;
    fontLable.setBold(false);
    fontLable.setPointSize(DEFAULT_NOTE_TEXT_SIZE);
    fontLable.setWeight(QFont::Normal);
    this->setFont(fontLable);
    // set text color
    QPalette pe = this->palette();
#ifdef _T49
    pe.setColor(QPalette::WindowText, DEFAULT_INFO_TEXT_COLOR);
#else
    pe.setColor(QPalette::WindowText, Qt::black);
#endif
    this->setPalette(pe);

    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
}

NoteLabel::~NoteLabel()
{
    if (NULL != m_pTimer)
    {
        if (m_pTimer->isActive())
        {
            m_pTimer->stop();
        }
        disconnect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
    }
}

void NoteLabel::SetLabelText(QString strNote)
{
    m_strDis = strNote;
    setText(m_strDis);
    if (INT_DISAPPEAR == m_eType && NULL != m_pTimer)
    {
        m_pTimer->start(NOTE_DISAPPEAR_TIME);
    }
    else if (INT_ANIMATION == m_eType && NULL != m_pTimer)
    {
        m_pTimer->start(NOTE_ANIMATION_TIME);
    }
}

void NoteLabel::SetLabelType(Item_Note_Type eType)
{
    if (eType < INT_END && eType >= INT_NONE && m_eType != eType)
    {
        if (m_pTimer->isActive())
        {
            m_pTimer->stop();
        }
        m_eType = eType;
    }
}

void NoteLabel::OnTimeOut()
{
    m_pTimer->stop();
    if (INT_DISAPPEAR == m_eType)
    {
        m_eType = INT_NONE;
        m_strDis.clear();
        this->hide();
    }
    else if (INT_ANIMATION == m_eType)
    {
        yl::string strNode = "";
        for (int i = 0; i < m_iCount; ++i)
        {
            strNode += ".";
        }
        m_iCount = (m_iCount + 1) % 4;

        QString strText = QString("%1%2").arg(m_strDis).arg(strNode.c_str());
        setText(strText);
        update();
        m_pTimer->start(NOTE_ANIMATION_TIME);
    }
}
