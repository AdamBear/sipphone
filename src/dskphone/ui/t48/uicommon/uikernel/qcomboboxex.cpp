#include <QtGui>
#include "qcomboboxex.h"
#include "languagehelper.h"
#include "qtcommon/qmisc.h"


SCROLL_TEXT_IMPLEMENTATION(qComboBoxEx, CurrentText, currentText(), GetCurrentTextWidth())


qComboBoxEx::qComboBoxEx(QWidget * parent)
    : QComboBox(parent)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSelecteChanged(int)));

    SCROLL_TEXT_INIT(CurrentText)
    SetScrollCurrentText(true);
}

qComboBoxEx::~qComboBoxEx()
{
    SCROLL_TEXT_UNINIT(CurrentText)
}

void qComboBoxEx::focusInEvent(QFocusEvent * pEvent)
{
    QComboBox::focusInEvent(pEvent);

    SCROLL_TEXT_CHECK(CurrentText)
}

void qComboBoxEx::focusOutEvent(QFocusEvent * pEvent)
{
    QComboBox::focusOutEvent(pEvent);

    SCROLL_TEXT_CHECK(CurrentText)
}

void qComboBoxEx::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    // draw the combobox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    // draw the icon and text
    SCROLL_TEXT_PAINT(painter, m_rcText, CurrentText, currentText())
}

void qComboBoxEx::resizeEvent(QResizeEvent * pEvent)
{
    QComboBox::resizeEvent(pEvent);

    QStylePainter stylePainter(this);
    QStyleOptionComboBox opt;

    initStyleOption(&opt);

    QStyle * pStyle = stylePainter.style();

    if (NULL == pStyle)
    {
        return;
    }

    m_rcText = pStyle->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
    m_rcText = m_rcText.adjusted(1, 0, -1, 0);
}

bool qComboBoxEx::IsFocus()
{
    return hasFocus();
}

int qComboBoxEx::GetCurrentTextWidth()
{
    return m_rcText.width();
}

void qComboBoxEx::OnSelecteChanged(int nIndex)
{
    SCROLL_TEXT_CHECK(CurrentText)
}

void qComboBoxEx::ScrollCurrentText()
{
    SCROLL_TEXT_INTERVAL(CurrentText);

    update(m_rcText);
}
