#include <QtGui>
#include <QLayout>
#include <QRadioButton>
#include <QGroupBox>
#include "uikernel/languagehelper.h"
#include <QMouseEvent>
#include <QStylePainter>
#include "imagemanager/modimagemanager.h"
#include <QtGui>
#include <QPixmap>
#include "baseui/t4xpicpath.h"
#include "qcomboboxlabel.h"
#include "qtcommon/qmisc.h"

#ifdef IF_FEATURE_COMBOBOXLABEL

qComboBoxLabel::qComboBoxLabel(QWidget * parent /*= 0*/)
    : QWidget(parent), m_fOffset(0), m_strLabel(),
      m_pRadioOn(NULL),
      m_pRadioOff(NULL)
{
    QFont itemFont;
    itemFont.setPointSize(16);
    itemFont.setBold(false);
    this->setFont(itemFont);

    m_pRadioOn = new QUIRadioButton(this);
    m_pRadioOff = new QUIRadioButton(this);

    //设置初始RadioButton文本
    m_pRadioOn->setText(LANG_TRANSLATE(TRID_ON));
    m_pRadioOff->setText(LANG_TRANSLATE(TRID_OFF));
    m_pRadioOn->setFixedSize(75, 45);
    m_pRadioOn->setFixedSize(75, 45);
    m_pRadioOn->setGeometry(0, 0, 75, 45);
    m_pRadioOff->setGeometry(80, 0, 75, 45);

    m_fOffset = m_pRadioOn->width() + m_pRadioOff->width() + 10;

    connect(m_pRadioOn, SIGNAL(clicked()), this, SLOT(OnSelectChange()));
    connect(m_pRadioOff, SIGNAL(clicked()), this, SLOT(OnSelectChange()));

    //设置大小
    this->setFixedHeight(45);
}

void qComboBoxLabel::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    QRect rectRight(m_fOffset, 0, width() - m_fOffset, height());
    painter.drawItemText(rectRight, (Qt::AlignLeft | Qt::AlignVCenter), palette(), isEnabled(),
                         m_strLabel);
}

qComboBoxLabel::~qComboBoxLabel()
{
    disconnect(m_pRadioOn, SIGNAL(clicked()), this, SLOT(OnSelectChange()));
    disconnect(m_pRadioOff, SIGNAL(clicked()), this, SLOT(OnSelectChange()));
}

void qComboBoxLabel::SetData(bool bFlag)
{
    bFlag ? m_pRadioOn->setChecked(true) : m_pRadioOff->setChecked(true);
    OnSelectChange();
}

bool qComboBoxLabel::GetData() const
{
    return m_pRadioOn->isChecked();
}

void qComboBoxLabel::SetContent(const QString & strTextOn, const QString & strTextOff)
{
    if (!strTextOn.isEmpty())
    {
        m_pRadioOn->setText(strTextOn);
    }
    if (!strTextOff.isEmpty())
    {
        m_pRadioOff->setText(strTextOff);
    }
}

QRadioButton * qComboBoxLabel::GetRadioButtonOn() const
{
    return m_pRadioOn;
}

void qComboBoxLabel::OnSelectChange()
{
    // this->setFocus(Qt::MouseFocusReason);
    emit checkChanged(GetData());
}

void qComboBoxLabel::focusInEvent(QFocusEvent * event)
{
    if (GetData())
    {
        m_pRadioOn->setFocus(Qt::ActiveWindowFocusReason);
    }
    else
    {
        m_pRadioOff->setFocus(Qt::ActiveWindowFocusReason);
    }
}

void qComboBoxLabel::SetLabel(const yl::string & strLabel)
{
    m_strLabel = toQString(strLabel);
}
#endif
