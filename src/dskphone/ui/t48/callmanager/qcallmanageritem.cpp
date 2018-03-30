#include "qcallmanageritem.h"
#include "QStylePainter"
#include "QPainter"
#include "QRect"
#include "QStyleOptionButton"
#include "imagemanager/modimagemanager.h"
#include "customtheme/modcustomtheme.h"

namespace
{
// 图片有尖角，长度为14，所以，画时首先偏移14
#define TOP_START_Y                                 14
// Icon 和 Text间隔
#define ICON_DISTANCE_TEXT                          0
}

qCallManagerBarItem::qCallManagerBarItem(QWidget * parent /* = 0 */)
    : QPushButton(parent)
    , m_iTopDis(0)
    , m_iLeftDis(0)
    , m_strSelectPic("")
    , m_strIcon("")
    , m_strText("")
    , m_strClickAction("")
    , m_bBg(false)
{
    connect(this, SIGNAL(clicked()), this, SLOT(OnClick()));


}

qCallManagerBarItem::~qCallManagerBarItem(void)
{
}

// 设置左偏距离
void qCallManagerBarItem::SetLeftDistance(const int iLeftDis)
{
    if (iLeftDis >= 0)
    {
        m_iLeftDis = iLeftDis;
    }
}

// 获取左偏距离
int qCallManagerBarItem::GetLeftDistance()
{
    return m_iLeftDis;
}

// 设置上偏距离
void qCallManagerBarItem::SetTopDistance(const int iTopDis)
{
    if (iTopDis >= 0)
    {
        m_iTopDis = iTopDis;
    }
}

// 获取上偏距离
int qCallManagerBarItem::GetTopDistance()
{
    return m_iTopDis;
}

// 设置字体font

// 设置Icon
void qCallManagerBarItem::SetIconPicture(const QString & strIconPic)
{
    if (!strIconPic.isNull())
    {
        m_strIcon = strIconPic;
    }
}

// 获取Icon
QString qCallManagerBarItem::GetIconPicture()
{
    if (!m_strIcon.isNull())
    {
        return m_strIcon;
    }
    else
    {
        return "";
    }
}

// 设置Text
void qCallManagerBarItem::SetText(const QString & strText)
{
    if (!strText.isNull())
    {
        m_strText = strText;
        setText(strText);
    }
}

// 获取Text
QString qCallManagerBarItem::GetText()
{
    if (!m_strText.isNull())
    {
        return m_strText;
    }
    else
    {
        return "";
    }
}

// 设置SelectPic
void qCallManagerBarItem::SetSelectPicture(const QString & strSelectPic)
{
    if (!strSelectPic.isNull())
    {
        m_strSelectPic = strSelectPic;
    }
}

// 获取SelectPic
QString qCallManagerBarItem::GetSelectPicture()
{
    if (!m_strSelectPic.isNull())
    {
        return m_strSelectPic;
    }
    else
    {
        return "";
    }
}

// 设置Action
void qCallManagerBarItem::SetClickAction(const QString & strAction)
{
    if (!strAction.isNull())
    {
        m_strClickAction = strAction;
    }
}

// 获取Action
QString qCallManagerBarItem::GetClickAction()
{
    if (!m_strClickAction.isNull())
    {
        return m_strClickAction;
    }
    else
    {
        return "";
    }
}

// 设置是否绘制Background
void qCallManagerBarItem::SetDrawBg(const bool bBg)
{
    m_bBg = bBg;
}

// 获取是否绘制Background
bool qCallManagerBarItem::GetDrawBg()
{
    return m_bBg;
}

void qCallManagerBarItem::OnClick()
{
    if (!m_strClickAction.isNull())
    {
        emit ClickAction(m_strClickAction);
    }
}

void qCallManagerBarItem::paintEvent(QPaintEvent * pPaintEvent)
{
    QStylePainter p(this);
    QPixmap pm;
    QStyleOptionButton option;
    initStyleOption(&option);

    QPalette pal = option.palette;
    QRect rc = this->rect();
    QRect rcSelectBg;
    QRect rcText = option.rect;
    QRect rcIcon;
    int iTopLeft = TOP_START_Y;

    // 设置字体大小
    QFont fontTitle = font();
    fontTitle.setPointSize(CustomTheme_GetCustomFontSizeByKey(CUSTOM_FONT_ID_SOFAKEY_NORMAL));
    p.setFont(fontTitle);

    // 画选中图片
    if (m_bBg && !m_strSelectPic.isNull() && !m_strSelectPic.isEmpty())
    {
        pm = THEME_GET_BMP(m_strSelectPic.toUtf8().data());
        if (!pm.isNull())
        {
            p.drawPixmap(rc, pm);
        }
    }

    // 画Icon
    if (!m_strIcon.isNull() && !m_strIcon.isEmpty())
    {
        //iTopLeft += m_iTopDis;
        pm = THEME_GET_BMP(m_strIcon.toUtf8().data());
        if (!pm.isNull())
        {
            iTopLeft += (rc.height() / 4 - pm.rect().height() / 2);
            rcIcon.setRect(rc.width() / 2 - pm.rect().width() / 2,
                           iTopLeft,
                           pm.rect().width(),
                           pm.rect().height());

            p.drawPixmap(rcIcon, pm);

            // 更新偏移
            iTopLeft += rcIcon.height();
        }
    }

    // 画Text
    if (!m_strText.isNull() && !m_strText.isEmpty())
    {
        iTopLeft += ICON_DISTANCE_TEXT;
        rcText.setY(iTopLeft);

        // 设置字体颜色
        QPen pen = p.pen();
        if (m_bBg)
        {
            pen.setColor(CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_SOFAKEY_NORMAL));
        }
        else
        {
            pen.setColor(CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_SOFAKEY_NORMAL));
        }
        p.setPen(pen);

        p.drawItemText(rcText, Qt::AlignHCenter | Qt::AlignTop, pal, this->isEnabled(), m_strText);
    }
}
