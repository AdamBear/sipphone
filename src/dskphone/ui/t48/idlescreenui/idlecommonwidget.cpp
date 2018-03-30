#include "idlecommonwidget.h"
#include "baseui/fontcolor.h"
#include "imagemanager/modimagemanager.h"

CNoteLabel::CNoteLabel(QWidget * parent /* = NULL */)
    : QLabel(parent)
    , m_strIcon("")
    , m_strText("")
{
    Init();
}

void CNoteLabel::Init()
{
    //
    this->setGeometry(0, 0, 320, 40);

    //
    SetWgtFontColor(this, FONT_TYPE_TEXT);
}

CNoteLabel::~CNoteLabel()
{

}

void CNoteLabel::SetLabelIcon(const QString & strIcon)
{
    m_strIcon = strIcon;
}

void CNoteLabel::SetLabelText(const QString & strText)
{
    m_strText = strText;
}

void CNoteLabel::paintEvent(QPaintEvent * paintEvent)
{
    QStylePainter painter(this);
    //
    QRect rcWidget = geometry();

    QRect rcPixmap = rcWidget;
    QPixmap pmPhoto = THEME_GET_BMP(m_strIcon.toUtf8().data());
    rcPixmap.setTop(rcPixmap.top()
                    + rcWidget.height() > pmPhoto.height()
                    ? (rcWidget.height() - pmPhoto.height()) / 2
                    : 0);
    rcPixmap.setTop(rcPixmap.top() + 3);
    rcPixmap.setLeft(15);
    rcPixmap.setHeight(pmPhoto.height());
    rcPixmap.setWidth(pmPhoto.width());

    //
    //painter.drawPixmap(rcPixmap, THEME_GET_BMP(m_strIcon.c_str()));
    //
    if (m_strIcon.isEmpty())
    {
        painter.drawItemText(QRect(0, 0, rcWidget.width(), rcWidget.height()),
                             Qt::AlignCenter, this->palette(), isEnabled(), m_strText);
    }
    else
    {
        painter.drawItemText(QRect(60, 5, 320 - 60, 30),
                             Qt::AlignLeft | Qt::AlignVCenter, this->palette(), isEnabled(), m_strText);
    }
}
