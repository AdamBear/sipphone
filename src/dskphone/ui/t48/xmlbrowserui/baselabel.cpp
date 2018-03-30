#include "baselabel.h"
#include "textstylemanager.h"
#include "qtcommon/qmisc.h"
namespace
{
#define  XB_BASE_lABEL_DEFAULT_FONT_SIZE 18
}

CBaseLabel::CBaseLabel(QWidget * parent /* = 0 */)
    : QLabel(parent)
    , m_bIsPressed(false)
{
    Init();
}

CBaseLabel::~CBaseLabel()
{
    //
}

void CBaseLabel::Init()
{
    //进行Label属性的初始化
    m_strTextContent = "";

    QFont font;
    font.setPointSize(XB_BASE_lABEL_DEFAULT_FONT_SIZE);
    font.setWeight(QFont::Bold);
    font.setBold(true);
    m_TextFont = font;

    m_TextColor = Qt::white;
    m_bIsWrap = false;
    m_nLabLength = LENGTH_LINE;
}

void CBaseLabel::mousePressEvent(QMouseEvent * pMouseEvent)
{
    m_bIsPressed = true;
}
void CBaseLabel::mouseReleaseEvent(QMouseEvent * pMouseEvent)
{
    if (m_bIsPressed)
    {
        emit clicked();
        m_bIsPressed = false;
    }

};

void CBaseLabel::SetTextContent(QString strText /* = */)
{
    m_strTextContent = strText;
}

void CBaseLabel::SetTextWrap(bool bWrap /* = false */)
{
    m_bIsWrap = bWrap;
}

void CBaseLabel::SetTextColor(QColor color /* = Qt::white */)
{
    m_TextColor = color;
}

void CBaseLabel::SetTextFont(QFont font)
{
    m_TextFont = font;
}

void CBaseLabel::SetLabLength(int nLength /* = LENGTH_LINE */)
{
    m_nLabLength = nLength;
}

int CBaseLabel::GetAutoResizeHeight()
{
    //根据label显示的文字属性及控件长度，计算label的高度
    if (!m_bIsWrap)         //不分行显示
    {
        //先设置文字效果
        this->setFont(m_TextFont);
        this->setWordWrap(false);

        //获取单行显示时的文本高度
        int iTextHeight = GetTextHeight(replaceLabelText(m_strTextContent, this));

        //单行显示时，label高度设置为文字高度加16
        iTextHeight = iTextHeight > XB_BASE_lABEL_MIN_HEIGHT ? iTextHeight : XB_BASE_lABEL_MIN_HEIGHT;
        return iTextHeight;
    }
    else
    {
        //事实上多行显示时一般采用默认字体
        this->setFont(m_TextFont);
        this->setWordWrap(true);

        int iTextHeight = GetTextHeight(m_strTextContent);
        int nCount = (iTextHeight - 1) / HEIGHT_LINE + 1;
        return HEIGHT_LINE * nCount;
        //return iTextHeight + 2;
    }
}

int CBaseLabel::GetTextHeight(const QString & strText)
{
    int flags = this->wordWrap() ? (int)(alignment() | Qt::TextWordWrap) : (int)alignment();

    int nHeight = this->wordWrap() ? XB_BASE_lABEL_MIN_HEIGHT : HEIGHT_LINE;
    //int segs =    this->fontMetrics().boundingRect(QRect(0, 0, m_nLabLength, HEIGHT_LINE), flags, strText).height();

    return this->fontMetrics().boundingRect(QRect(0, 0, m_nLabLength, nHeight), flags,
                                            strText).height();
}

void CBaseLabel::UpdateDiaplay()
{
    //根据显示风格进行显示
    this->setWordWrap(m_bIsWrap);
    this->setFont(m_TextFont);

    //设置文字颜色
    QPalette pal = this->palette();
    pal.setColor(QPalette::WindowText, m_TextColor);
    this->setPalette(pal);

    //默认label长度为LENGTH_LINE
    this->resize(m_nLabLength, GetAutoResizeHeight());

    this->setText(replaceLabelText(m_strTextContent, this));

    this->show();
}

void CBaseLabel::ClearDisplay()
{
    Init();
    UpdateDiaplay();
}

bool CBaseLabel::eventFilter(QObject * pObj, QEvent * pEvent)
{
    //
    return false;
}

