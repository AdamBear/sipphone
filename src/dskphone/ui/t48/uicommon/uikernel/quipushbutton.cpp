#include "quipushbutton.h"
#include "imagemanager/modimagemanager.h"
#include <QtGui>

qUIPushButton::qUIPushButton(QWidget * parent /* = 0 */) :
    QPushButton(parent),
    m_eIconAlign(Qt::AlignCenter),
    m_eAlign(Qt::AlignCenter),
    m_clrText(Qt::black),
    m_isTransparent(false),
    m_iIconXOffSet(0),
    m_iTextXOffSet(0),
    m_iIconYOffSet(0),
    m_iTextYOffSet(0),
    m_iBtnID(-1)
{
    this->setObjectName("qUIPushButton");
    this->setFocusPolicy(Qt::NoFocus);

    connect(this, SIGNAL(clicked()), this, SLOT(OnButtonClicked()));
    connect(this, SIGNAL(pressed()), this, SLOT(OnButtonPressed()));
    connect(this, SIGNAL(released()), this, SLOT(OnButtonReleased()));
}

qUIPushButton::~qUIPushButton()
{

}

void qUIPushButton::setTransparent(bool is_transparent /* = false */)
{
    // 设置属性和本身属性一样时不进行设置
    if (m_isTransparent != is_transparent)
    {
        m_isTransparent = is_transparent;

        // 设置透明或非透明
        QPalette pal = palette();
        if (m_isTransparent)
        {
            pal.setColor(QPalette::Window, QColor(0, 0, 0, 0));
        }
        else
        {
            pal.setColor(QPalette::Window, QColor(255, 255, 255, 255));
        }

        setPalette(pal);
    }
}

void qUIPushButton::setTextColor(const QColor & clr)
{
    // 设置属性和本身属性一样时不进行设置
    if (clr != m_clrText)
    {
        m_clrText = clr;

        QPalette pal = palette();
        pal.setColor(QPalette::Active, QPalette::ButtonText, m_clrText);
        pal.setColor(QPalette::Inactive, QPalette::ButtonText, m_clrText);
        pal.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::lightGray);
        setPalette(pal);
    }
}

void qUIPushButton::OnButtonPressed()
{
    emit pressed(m_strBtnAction);
}

void qUIPushButton::OnButtonReleased()
{
    emit released(m_strBtnAction);
}

void qUIPushButton::OnButtonClicked()
{
    emit clicked(m_strBtnAction);
}

void qUIPushButton::paintEvent(QPaintEvent * pe)
{
    QStylePainter p(this);
    QPixmap pm;

    // 判断button的状态，设置对应的图片
    if (!this->isEnabled())
    {
        pm = THEME_GET_BMP(m_strDisalbePic.toUtf8().data());
    }
    else if (this->isDown()
             || this->isChecked())
    {
        pm = THEME_GET_BMP(m_strEnablePic.toUtf8().data());
    }
    else
    {
        pm = THEME_GET_BMP(m_strNormalPic.toUtf8().data());
    }

    QStyleOptionButton option;
    initStyleOption(&option);

    // 画按钮背景图片
    if (!pm.isNull())
    {
        p.drawPixmap(option.rect.x() + (option.rect.width() - pm.width()) / 2,
                     option.rect.y() + option.rect.height() / 2 - pm.height() / 2, pm);
    }

    // 画按钮上的ICON
    QPixmap pmIcon;
    bool hasDrawnIcon = false;
    // 判断Icon的对齐方式.目前只有居左、居右、居中三种情况
    bool isIconAlignLeft = (m_eIconAlign & Qt::AlignLeft) == Qt::AlignLeft;
    bool isIconAlignRight = (m_eIconAlign & Qt::AlignRight) == Qt::AlignRight;
    bool isIconAlignCenter = (m_eIconAlign & Qt::AlignCenter) == Qt::AlignCenter;
    bool isIconAlignTop = (m_eIconAlign & Qt::AlignTop) == Qt::AlignTop;
    bool isIconAlignBottom = (m_eIconAlign & Qt::AlignBottom) == Qt::AlignBottom;

    QRect rcIcon;
    if (!m_strIconPic.isEmpty())
    {
        pmIcon = THEME_GET_BMP(m_strIconPic.toUtf8().data());
        if (!pmIcon.isNull())
        {
            // 通过Icon的对齐方式，计算绘制的x坐标
            int x = 0;
            int y = 0;
            if (isIconAlignLeft)
            {
                x = option.rect.x() + m_iIconXOffSet;
                y = option.rect.y() + option.rect.height() / 2 - pmIcon.height() / 2;
            }
            else if (isIconAlignRight)
            {
                x = option.rect.x() + option.rect.width() - pmIcon.width() - m_iIconXOffSet;
                y = option.rect.y() + option.rect.height() / 2 - pmIcon.height() / 2;
            }
            else if (isIconAlignCenter)
            {
                x = option.rect.x() + (option.rect.width() - pmIcon.width()) / 2;
                y = option.rect.y() + option.rect.height() / 2 - pmIcon.height() / 2;
            }
            else if (isIconAlignTop)
            {
                x = option.rect.x() + (option.rect.width() - pmIcon.width()) / 2;
                y = option.rect.y() + m_iIconYOffSet;
            }
            else if (isIconAlignBottom)
            {
                x = option.rect.x() + (option.rect.width() - pmIcon.width()) / 2;
                y = option.rect.y() + option.rect.height() - pmIcon.height() - m_iIconYOffSet;
            }

            // 判断Icon坐标是否为有效坐标
            if (x >= 0 && x < option.rect.x() + option.rect.width() &&
                    y >= 0 && y < option.rect.y() + option.rect.height())
            {
                rcIcon.setRect(x, y, pmIcon.width(), pmIcon.height());

                // 坐标有效绘制Icon
                p.drawPixmap(rcIcon, pmIcon);

                // 设置绘制成功的标志位
                hasDrawnIcon = true;
            }
        }
    }

    QString tx = this->text();

    // 若是Text为空，就没必要绘制
    if (!tx.isEmpty())
    {
        // 画按钮上的文本, 如果已经画了ICON, 则需要进行相应的偏移
        QRect rcText = option.rect;

        if (isIconAlignLeft)
        {
            // +m_iTextXOffSet 在ICON和文字之间留一点空隙
            rcText.setX(rcText.x()  + rcIcon.right() + m_iTextXOffSet);
        }
        else if (isIconAlignRight)
        {
            // -m_iTextXOffSet 在ICON和文字之间留一点空隙
            rcText.setWidth(rcText.width() - rcIcon.right() - m_iTextXOffSet);
        }
        else if (isIconAlignTop)
        {
            // 在ICON和文字之间留一点空隙
            rcText.setY(rcText.height() - rcIcon.bottom() + m_iTextYOffSet);
        }
        else if (isIconAlignBottom)
        {
            // 在ICON和文字之间留一点空隙
            rcText.setHeight(rcText.height() - rcIcon.top() - m_iTextYOffSet);
        }

        // 绘制Item的Text
        p.drawItemText(rcText, m_eAlign, option.palette, this->isEnabled(), tx);
    }

    // 所有东西绘制完后,才绘制上遮罩
    if (this->isDown()
            || this->isChecked())
    {
        QPixmap pmMask = THEME_GET_BMP(m_strMaskPic.toUtf8().data());
        if (!pmMask.isNull())
        {
            p.drawPixmap(option.rect.x() + (option.rect.width() - pmMask.width()) / 2,
                         option.rect.y() + (option.rect.height() - pmMask.height()) / 2, pmMask);
        }
    }
}

bool qUIPushButton::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (pEvent == NULL)
    {
        return false;
    }

    //这个事件已经没必要捕捉， 所以在构造函数中取消
    //这个事件是针对 有模式对话框的弹出框 处理的   目前这个已经取消
    //优化速度 所以不注册过滤器
    if (pEvent->type() == QEvent::WindowDeactivate)
    {
        //释放按下效果
        this->setDown(false);

        //停止长按计时器
        DoAfterRelease();
    }
    return false;
}

qUIPushButton * CreateUIPushButton(QRect rect, QString strObjectName, QWidget * parent)
{
    qUIPushButton * pBtn = new qUIPushButton(parent);
    if (pBtn != NULL)
    {
        pBtn->setGeometry(rect);

        pBtn->setObjectName(strObjectName);
    }

    return pBtn;
}
