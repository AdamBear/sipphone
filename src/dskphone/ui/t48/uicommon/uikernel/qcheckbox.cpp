#include "qcheckbox.h"
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

QUIRadioButton::QUIRadioButton(QWidget * parent)
    : QRadioButton(parent),
      m_bDown(false)
{

}

QUIRadioButton::~QUIRadioButton()
{

}

// 鼠标放开事件响应函数
void QUIRadioButton::mousePressEvent(QMouseEvent * e)
{
    m_bDown = true;
}

// 鼠标放开事件响应函数
void QUIRadioButton::mouseReleaseEvent(QMouseEvent * e)
{
    if (m_bDown)
    {
        QPoint nPos = this->mapToGlobal(QPoint(0, 0));
        QSize nSize = this->size();
        QPoint nMPos = e->globalPos();
        // 判断抬起位置是否在控件内
        if (nMPos.x() > nPos.x() && nMPos.y() > nPos.y() && nMPos.x() < (nPos.x() + nSize.width())
                && nMPos.y() < (nPos.y() + nSize.height()))
        {
            setChecked(true);
            emit clicked();
        }
    }
    m_bDown = false;
}

void QUIRadioButton::paintEvent(QPaintEvent * pe)
{
    QStylePainter p(this);
    QPixmap pm;
    QStyleOptionButton option;
    initStyleOption(&option);
    QPalette pal = option.palette;
    // 设置文字颜色为黑色
    pal.setColor(QPalette::Text, qRgb(0, 0, 0));
    // 设置字体大小
    QFont fontTitle = font();
    fontTitle.setPointSize(16);
    p.setFont(fontTitle);
    // 显示的图标 和 文字颜色

    if (this->isChecked())
    {
        if (this->hasFocus())
        {
            pm = THEME_GET_BMP(PIC_RADIO_ON_FOCUS);
        }
        else
        {
            pm = THEME_GET_BMP(PIC_RADIO_ON);
        }
    }
    else
    {
        if (this->hasFocus())
        {
            pm = THEME_GET_BMP(PIC_RADIO_OFF_FOCUS);
        }
        else if (this->isEnabled())
        {
            pm = THEME_GET_BMP(PIC_RADIO_OFF);
        }
        else
        {
            pm = THEME_GET_BMP(PIC_RADIO_DISABLE);
        }
    }

    QRect rcIcon;
    // Icon的Y坐标默认居中
    int y = option.rect.y() + option.rect.height() / 2 - pm.height() / 2;
    rcIcon.setRect(option.rect.x() + 10, y, pm.width(), pm.height());
    // 画按钮背景图片
    if (!pm.isNull())
    {
        p.drawPixmap(rcIcon, pm);
    }
    QString tx = this->text();

    // 若是Text为空，就没必要绘制
    if (tx.isEmpty())
    {
        return ;
    }
    // 画按钮上的文本, 如果已经画了ICON, 则需要进行相应的偏移
    QRect rcText = option.rect;
    // 默认多偏移一个icon的宽度
    rcText.setX(rcText.x()  +  pm.width() + 12);
    // 绘制Item的Text
    p.drawItemText(rcText, Qt::AlignVCenter | Qt::AlignLeft, pal, this->isEnabled(), tx);

}

qCheckBox::qCheckBox(QWidget * parent /* ==NULL */)
    : QWidget(parent),
      m_pRadioOn(NULL),
      m_pRadioOff(NULL)
{
    QFont itemFont;
    itemFont.setPointSize(16);
    itemFont.setBold(false);
    this->setFont(itemFont);

    m_pRadioOn = new QUIRadioButton(this);
    m_pRadioOff = new QUIRadioButton(this);

    //指针判空
    if (m_pRadioOff == NULL || m_pRadioOn == NULL)
    {
        return ;
    }

    m_pRadioOn->setObjectName("RadioOn");
    m_pRadioOff->setObjectName("RadioOff");

    //设置初始RadioButton文本
    m_pRadioOn->setText(LANG_TRANSLATE(TRID_ON));
    m_pRadioOff->setText(LANG_TRANSLATE(TRID_OFF));
    m_pRadioOn->setFixedSize(100, 45);
    m_pRadioOn->setFixedSize(100, 45);
    m_pRadioOn->setGeometry(0, 0, 100, 45);
    m_pRadioOff->setGeometry(110, 0, 100, 45);

    connect(m_pRadioOn, SIGNAL(clicked()), this, SLOT(OnSelectChange()));
    connect(m_pRadioOff, SIGNAL(clicked()), this, SLOT(OnSelectChange()));

    //设置大小
    this->setFixedSize(210, 45);
}

qCheckBox::~qCheckBox()
{
    disconnect(m_pRadioOn, SIGNAL(clicked()), this, SLOT(OnSelectChange()));
    disconnect(m_pRadioOff, SIGNAL(clicked()), this, SLOT(OnSelectChange()));
}

void qCheckBox::SetData(bool bFlag)
{
    if (m_pRadioOn == NULL)
    {
        m_pRadioOn = new QUIRadioButton(this);
    }
    if (m_pRadioOff == NULL)
    {
        m_pRadioOff = new QUIRadioButton(this);
    }
    bFlag ? m_pRadioOn->setChecked(true) : m_pRadioOff->setChecked(true);
    OnSelectChange();
}

bool qCheckBox::GetData() const
{
    if (m_pRadioOn == NULL)
    {
        return false;
    }
    return m_pRadioOn->isChecked();
}

void qCheckBox::SetContent(const QString & strTextOn, const QString & strTextOff)
{
    if (m_pRadioOn == NULL)
    {
        m_pRadioOn = new QUIRadioButton(this);
    }
    if (m_pRadioOff == NULL)
    {
        m_pRadioOff = new QUIRadioButton(this);
    }

    if (!strTextOn.isEmpty())
    {
        m_pRadioOn->setText(strTextOn);
    }
    if (!strTextOff.isEmpty())
    {
        m_pRadioOff->setText(strTextOff);
    }
}

QRadioButton * qCheckBox::GetRadioButtonOn() const
{
    return m_pRadioOn;
}

void qCheckBox::OnSelectChange()
{
    // this->setFocus(Qt::MouseFocusReason);
    emit checkChanged(GetData());
}

void qCheckBox::focusInEvent(QFocusEvent * event)
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
