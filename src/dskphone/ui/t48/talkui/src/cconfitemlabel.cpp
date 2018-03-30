#include "cconfitemlabel.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"

CConfItemLabel::CConfItemLabel(QWidget * parent /* = 0 */)
    : QLabel(parent)
    , m_pHeldLabel(NULL)
    , m_pHoldLabel(NULL)
    , m_pHDLabel(NULL)
{
    QHBoxLayout * layout = new QHBoxLayout(this);

    m_pHeldLabel = new QLabel(this);
    if (NULL != m_pHeldLabel)
    {
        m_pHeldLabel->setAlignment(Qt::AlignCenter);
        m_pHeldLabel->setMaximumSize(28, 28);
        m_pHeldLabel->setMinimumSize(28, 28);
        m_pHeldLabel->setScaledContents(true);
        m_pHeldLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_pHeldLabel->hide();
    }

    m_pHoldLabel = new QLabel(this);
    if (NULL != m_pHoldLabel)
    {
        m_pHoldLabel->setAlignment(Qt::AlignCenter);
        m_pHoldLabel->setMaximumSize(28, 28);
        m_pHoldLabel->setMinimumSize(28, 28);
        m_pHoldLabel->setScaledContents(true);
        m_pHoldLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_pHoldLabel->hide();
    }

    m_pHDLabel = new QLabel(this);
    if (NULL != m_pHDLabel)
    {
        SetWgtFontColor(m_pHDLabel, FONT_TYPE_TEXT);
        m_pHDLabel->setAlignment(Qt::AlignCenter);
        m_pHDLabel->setMaximumWidth(40);
        m_pHDLabel->setMinimumWidth(30);
        m_pHDLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_pHDLabel->hide();
    }

    if (NULL != layout)
    {
        layout->setMargin(0);
        layout->setSpacing(3);
        layout->insertStretch(0);
        layout->addWidget(m_pHeldLabel);
        layout->addWidget(m_pHoldLabel);
        layout->addWidget(m_pHDLabel);
    }
}

CConfItemLabel::~CConfItemLabel()
{
    //
}

void CConfItemLabel::SetContent(const yl::string & strHDIcon,
                                const yl::string & strHoldIcon /* = */, const yl::string & strHeldIcon /* = */)
{
    SetHDIcon(strHDIcon);
    SetHoldIcon(strHoldIcon);
    SetFarMuteIcon(strHeldIcon);
}

void CConfItemLabel::ClearContent()
{
    SetHDIcon("");
    SetHoldIcon("");
    SetFarMuteIcon("");
}

void CConfItemLabel::SetHDIcon(const yl::string & strHDIcon)
{
    QPixmap mPixmap = THEME_GET_BMP(strHDIcon.c_str());
    m_pHDLabel->setPixmap(mPixmap);
    if (mPixmap.isNull())
    {
        m_pHDLabel->hide();
    }
    else
    {
        m_pHDLabel->show();
    }
}

void CConfItemLabel::SetHoldIcon(const yl::string & strHoldIcon /* = */)
{
    QPixmap mPixmap = THEME_GET_BMP(strHoldIcon.c_str());
    m_pHoldLabel->setPixmap(mPixmap);
    if (mPixmap.isNull())
    {
        m_pHoldLabel->hide();
    }
    else
    {
        m_pHoldLabel->show();
    }
}

void CConfItemLabel::SetFarMuteIcon(const yl::string & strHeldIcon /* = */)
{
    QPixmap mPixmap = THEME_GET_BMP(strHeldIcon.c_str());
    m_pHeldLabel->setPixmap(mPixmap);
    if (mPixmap.isNull())
    {
        m_pHeldLabel->hide();
    }
    else
    {
        m_pHeldLabel->show();
    }
}

bool CConfItemLabel::IsShowHDIcon() const
{
    if (m_pHDLabel != NULL)
    {
        return NULL != m_pHDLabel->pixmap() && !m_pHDLabel->pixmap()->isNull();
    }
    return false;
}

bool CConfItemLabel::IsShowHoldIcon() const
{
    if (m_pHoldLabel != NULL)
    {
        return NULL != m_pHoldLabel->pixmap() && !m_pHoldLabel->pixmap()->isNull();
    }
    return false;
}

bool CConfItemLabel::IsShowFarMuteIcon() const
{
    if (m_pHeldLabel != NULL)
    {
        return NULL != m_pHeldLabel->pixmap() && !m_pHeldLabel->pixmap()->isNull();
    }
    return false;
}
