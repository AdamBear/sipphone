#include "cdialbutton.h"
#include "qtcommon/qmisc.h"

/*****************************************************
 *函数名：CDialButton
 *函数描述：构造函数，初始化成员变量，连接信号槽
 *
 ****************************************************/
CDialButton::CDialButton(QWidget * parent /* = 0 */):
    qUIPushButton(parent),
    m_iMarginLeft(10),
    m_iMarginTop(30),
    m_iMarginBetweenDigitAndLetters(10),
    m_bAutoMarginHorizontal(false),
    m_bAutoMarginVertical(true),
    m_iDigitMarginLeftCal(30),
    m_iLetterMarginLeftCal(50),
    m_iMarginTopCal(30)
{
    m_fontDigit = QFont(this->font());
    m_fontLetter = QFont(this->font());
    m_iKey = 0;

    // 连接相关的信号和槽
    connect(this, SIGNAL(pressed()), this, SLOT(OnButtonPressed()));
    connect(this, SIGNAL(released()), this, SLOT(OnButtonReleased()));
}


CDialButton::~CDialButton()
{

}


/*****************************************************
 * 函数名：SetText
 * 函数描述：设置按键显示的数字和字母
 *
 *      [in] digit  数字
 *      [in] letters 字母
 *
 *返回：void
 ****************************************************/
void CDialButton::SetText(const QString & digit, const QString & letters /* = QString() */)
{
    m_qstrDigit = digit;
    m_qstrLetters = letters;
}


/****************************************************
 *函数名：SetFontSize
 *函数描述：设置数字和字母的字体
 *      [in] digit_size 数字大小
 *      [in] letter_size 字母大小
 *
 *返回：void
 ***************************************************/
void CDialButton::SetFontSize(const int & digit_size /* = 30 */, const int & letter_size /* = 14 */)
{
    m_fontDigit.setPointSize(digit_size);
    m_fontLetter.setPointSize(letter_size);

}

/****************************************************
 *函数名：SetMargin
 *函数描述：设置数字左对齐和上对齐
 *      [in] left 左对齐值
 *      [in] top 上对齐值
 *
 *返回：void
 * *************************************************/
void CDialButton::SetMargin(const int & left /* = 10 */, const int & top /* = 10 */)
{
    m_iMarginLeft = left;
    m_iMarginTop = top;
}


/**************************************************
 *函数名：SetMarginBetweenDigitAndLetters
 *函数描述：设置数字和字母之间的间隙
 *      [in] val 间隙值
 *
 * 返回：void
 **************************************************/
void CDialButton::SetMarginBetweenDigitAndLetters(const int & val /* = 10 */)
{
    m_iMarginBetweenDigitAndLetters = val;

}


/*****************************************************
 *函数名：EnableAutoMargin
 *函数描述：使能水平和竖直自动对齐
 *      [in] horizontal  水平对齐标志
 *      [in] vertical    竖直对齐标志
 *
 *返回：void
 *****************************************************/
void CDialButton::EnableAutoMargin(const bool & horizontal /* = false */,
                                   const bool & vertical /* = true */)
{
    m_bAutoMarginHorizontal = horizontal;
    m_bAutoMarginVertical = vertical;
}


/*****************************************************
 *函数名：SetKey
 *函数描述：设置按钮对应的键值
 *      [in] key  按键键值
 *
 * 返回：void
 * **************************************************/
void CDialButton::SetKey(const int & key)
{
    m_iKey = key;
}


/***************************************************
 *函数名：Init
 *函数描述：根据用户设置，计算数字、字母的画图位置。该函数在绘制前，必须被调用
 *      [in] void
 *
 *返回：void
 **************************************************/
void CDialButton::Init()
{
    m_iDigitMarginLeftCal = m_iMarginLeft;
    m_iMarginTopCal = m_iMarginTop;

    QFontMetrics fm1(m_fontDigit);
    QRect temp = fm1.tightBoundingRect(m_qstrDigit);
    int w_d = temp.width(), h_d = temp.height();
    int w_window = (this->size()).width(), h_window = (this->size()).height();

    if (m_bAutoMarginHorizontal)
    {
        m_iDigitMarginLeftCal = (w_window - w_d) / 2;
    }

    m_iLetterMarginLeftCal = m_iDigitMarginLeftCal + w_d + m_iMarginBetweenDigitAndLetters;

    if (m_bAutoMarginVertical)
    {
        m_iMarginTopCal = h_window - (h_window - h_d) / 2;
    }

}


/********************************************************
 *函数名：paintEvent
 *函数描述：重写画图函数，绘制3态图片，数值，字母
 *      [in]  pe    绘图事件
 *
 *返回：void
 *******************************************************/
void CDialButton::paintEvent(QPaintEvent * pe)
{
    // 所有东西绘制完后,才绘制上遮罩
    if (this->isDown())
    {
        QPainter p(this);
        QStyleOptionButton option;

        initStyleOption(&option);
        QPixmap pmMask = THEME_GET_BMP(m_strMaskPic.toUtf8().data());
        if (!pmMask.isNull())
        {
            p.drawPixmap(option.rect.x() + (option.rect.width() - pmMask.width()) / 2,
                         option.rect.y() + (option.rect.height() - pmMask.height()) / 2, pmMask);
        }
    }
}


/**************************************************
 *函数名：OnButtonClicked
 *函数描述： 响应clicked(), 发出对应键值信号
 *      [in] void
 *
 * 返回：void
 ************************************************/
void CDialButton::OnButtonPressed()
{
    emit BtnPressed(m_iKey);
}

void CDialButton::OnButtonReleased()
{
    emit BtnReleased(m_iKey);
}

