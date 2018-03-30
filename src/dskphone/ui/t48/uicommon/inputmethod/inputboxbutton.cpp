#include "inputboxbutton.h"

#include "mainwnd/modmainwnd.h"
#include "uimanager/rectdefine.h"

#include "inputmanagerdefine.h"
#include "qtcommon/qmisc.h"
#include "dsklog/log.h"

#include "baseui/modbaseui.h"
#include "uimanager/moduimanager.h"
#include "uimanager/basemainwnd.h"
#include "datastruct.h"

namespace
{
//向外延伸的有效范围
#define INPUT_BOX_EXTEND_WIDTH                      40

//外框的高
#define INPUT_BOX_HEIGHT                            50

//单个按钮（高亮部分）的高
#define INPUT_BOX_BUTTON_HEIGHT                     50

#define INPUT_BOX_HEIGHT_SPACE                      10

//左侧圆角图片边缘透明部分宽度
#define INPUT_BOX_BUTTON_LEFT_BG_EDGE           5

#ifdef IF_LYNC
//背景图片圆角宽度
#define INPUT_BOX_BG_FILLET_WIDTH                   0
//高亮按钮与边缘的间距的一半，判断焦点位置时使用
#define INPUT_BOX_EDGE_WIDTH_HALF                   9
#endif

//高亮按钮与边缘的间距
#define INPUT_BOX_EDGE_WIDTH                            6
//高亮图片圆角的宽度
#define INPUT_BOX_HIGHLIGHT_FILLET_WIDTH        0

#define INPUT_BOX_CHAR_COLOR                    QColor(55, 55, 55)
#define INPUT_BOX_CHAR_COLOR_HL                 QColor(255, 255, 255)

#define INPUT_BOX_CHAR_FONT_SIZE                    26

#define INPUT_BOX_CHAR_MIN_WIDTH                    60
#define INPUT_BOX_CHAR_MAX_WIDTH                    120

#define INPUT_BOX_CHAR_VER_MIN_WIDTH                120
#define INPUT_BOX_CHAR_VER_MAX_WIDTH                240


#define PIC_INPUTMETHOD_PATH PIC_COMMON_PATH "inputmethod/"

#ifdef IF_LYNC
#define INPUT_BOX_BG_LEFT                           PIC_INPUTMETHOD_PATH "inputbox_bg_left.png"
#define INPUT_BOX_BG_RIGHT                          PIC_INPUTMETHOD_PATH "inputbox_bg_right.png"
#define INPUT_BOX_BG_MIDDLE                         PIC_INPUTMETHOD_PATH "inputbox_bg_middle.png"

#define INPUT_BOX_HIGHLIGHT_LEFT                    PIC_INPUTMETHOD_PATH "inputbox_highlight_left.png"
#define INPUT_BOX_HIGHLIGHT_MIDDLE                  PIC_INPUTMETHOD_PATH "inputbox_highlight_middle.png"
#define INPUT_BOX_HIGHLIGHT_RIGHT                   PIC_INPUTMETHOD_PATH "inputbox_highlight_right.png"
#endif

#define INPUT_BOX_BG                                PIC_INPUTMETHOD_PATH "inputbox_bg.9.png"

#define INPUT_BOX_SHOW_MAX_NUM_ROW                  5

#define INPUT_BOX_LYNC_LIGHT_BUTTON_COLOR           QColor(0x6b,0x93,0xc0)

#ifdef IF_LYNC
#define INPUT_BOX_LYNC_MARGIN_COLOR                 QColor(55,55,55)
#define INPUT_BOX_LYNC_BG_COLOR                     QColor(255, 255, 255)
#endif
}

CInputBoxButton::CInputBoxButton()
    : QWidget(NULL)
    , m_pWidget(NULL)
    , m_bIsShow(false)
    , m_nCount(0)
    , m_nTotalWidth(0)
    , m_nIndex(KEYBOARD_INPUTBOX_ERROR_INDEX)
    , m_bNeedFullScreenUpdate(false)
    , m_bLastFullScreenUpdate(false)
#ifndef IF_LYNC
    , m_nBoxType(INPUT_BOX_HORIZONTAL_TYPE)
    , m_nShowMaxNumRow(INPUT_BOX_SHOW_MAX_NUM_ROW)
#endif
{
    InitInputBox();
}

CInputBoxButton::~CInputBoxButton()
{

}

CInputBoxButton& CInputBoxButton::GetInstance()
{
    static CInputBoxButton InputBoxButton;
    return InputBoxButton;
}

void CInputBoxButton::InitInputBox()
{
    this->setWindowFlags(Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    this->setParent(UIManager_GetMainWnd());
    hide();

    m_vecInputBoxString.clear();
    m_vecInputBoxWidth.clear();
    m_charFont.setPointSize(INPUT_BOX_CHAR_FONT_SIZE);
}

void CInputBoxButton::ResetInputBox()
{
    m_bIsShow = false;
    m_nCount = 0;
    m_nTotalWidth = 0;
    m_nIndex = KEYBOARD_INPUTBOX_ERROR_INDEX;
    m_vecInputBoxString.clear();
    m_vecInputBoxWidth.clear();
    m_nBoxType = INPUT_BOX_HORIZONTAL_TYPE;
    m_nShowMaxNumRow = INPUT_BOX_SHOW_MAX_NUM_ROW;

    INPUTMETHOD_INFO("CInputBoxButton::ResetInputBox");
}

void CInputBoxButton::ShowInputBox()
{
    m_bIsShow = true;
    show();
    raise();
}

void CInputBoxButton::HideInputBox()
{
    m_bNeedFullScreenUpdate = true;

    ///m_bLastFullScreenUpdate = BaseUI_IsFullScreenUpdate();
    ///BaseUI_SetFullScreenUpdate(true);

    m_bIsShow = false;
    hide();
}

void CInputBoxButton::SetInputBox(const QPoint& ptButtonLT, const vecInputBoxString& vChar, int nBoxtType, int nIndex)
{
    m_vecInputBoxString.clear();
    m_vecInputBoxWidth.clear();

    m_nTotalWidth = 0;
    m_nCount = vChar.size();
    if (m_nCount <= 0)
    {
        return;
    }

#ifndef IF_LYNC
    m_nIndex = nIndex;
    if (0 > m_nIndex || m_nIndex > m_nCount)
    {
        m_nIndex = 0;
    }

    m_nBoxType = nBoxtType;
    if (m_nBoxType == INPUT_BOX_VERTICAL_TYPE)
    {
        m_nShowMaxNumRow = 1;
    }
    else
    {
        m_nShowMaxNumRow = INPUT_BOX_SHOW_MAX_NUM_ROW;
    }
#endif
    int nCount = 0;
    QString strChar = "";

    for (; nCount < m_nCount; ++nCount)
    {
        strChar = vChar[nCount];
        //计算出该备选项的文字显示宽度
        int nCharWidth = CalculateTextWidth(strChar);

        //每个宽度实际包含左侧的空位置和两侧圆角宽度
        nCharWidth += INPUT_BOX_EDGE_WIDTH + INPUT_BOX_HIGHLIGHT_FILLET_WIDTH + INPUT_BOX_HIGHLIGHT_FILLET_WIDTH;
#ifndef IF_LYNC
        if (m_nBoxType == INPUT_BOX_VERTICAL_TYPE)
        {
            //如果是竖直排列(计算从按下字符高度到话机顶部是否够显示)
            if (ptButtonLT.y() - INPUT_BOX_HEIGHT * (nCount + 1) - INPUT_BOX_HEIGHT_SPACE > 0)
            {
                //记录最大的width
                m_nTotalWidth = m_nTotalWidth > nCharWidth ? m_nTotalWidth : nCharWidth;
                m_vecInputBoxString.push_back(strChar);
                m_vecInputBoxWidth.push_back(nCharWidth);
            }
            else
            {
                //否则结束添加,后面就不再显示
                break;
            }
        }
        else
        {
#endif
            //如果加上这一个，总长度还小于屏幕宽度，则可以加上
            if (m_nTotalWidth + nCharWidth + INPUT_BOX_EDGE_WIDTH <= PHONE_WIDTH)
            {
                //如果是最后一个，需要加上右侧的空位
                if (nCount == m_nCount - 1)
                {
                    nCharWidth += INPUT_BOX_EDGE_WIDTH;
                }

                m_vecInputBoxString.push_back(strChar);

                m_nTotalWidth += nCharWidth;
                m_vecInputBoxWidth.push_back(nCharWidth);
            }
            //否则结束添加
            else
            {
                if (nCount > 1 && nCount <= m_vecInputBoxWidth.size())
                {
                    //最后一个补加上右侧的空位
                    m_vecInputBoxWidth[nCount - 1] += INPUT_BOX_EDGE_WIDTH;
                    m_nTotalWidth += INPUT_BOX_EDGE_WIDTH;
                }
                break;
            }
#ifndef IF_LYNC
        }
#endif
    }

    //记录最终实际显示的个数
    m_nCount = nCount;
#ifndef IF_LYNC
    int nRow = 1;
    if (m_nBoxType == INPUT_BOX_VERTICAL_TYPE)
    {
        for (int i = 0; i < m_vecInputBoxWidth.size(); i++)
        {
            //竖直排序，以竖直方向最大的宽度进行显示
            m_vecInputBoxWidth[i] = m_nTotalWidth;
        }

        nRow = m_vecInputBoxWidth.size();
    }
    else
    {
        //显示大于m_nShowMaxNumRow(默认显示5)个，需要分行显示
        if (m_nCount > m_nShowMaxNumRow)
        {
            //大于m_nShowMaxNumRow个分下一行显示了，直接计算第一行宽度即可
            m_nTotalWidth = 0;
            for (int i = 0; i < m_vecInputBoxWidth.size() && i < m_nShowMaxNumRow; i++)
            {
                m_nTotalWidth += m_vecInputBoxWidth[i];
            }

            if (m_nCount % m_nShowMaxNumRow == 0)
            {
                nRow = m_nCount / m_nShowMaxNumRow;
            }
            else
            {
                nRow = m_nCount / m_nShowMaxNumRow + 1;
            }
        }
    }

    m_rctBox.setTop(ptButtonLT.y() - INPUT_BOX_HEIGHT * nRow - INPUT_BOX_HEIGHT_SPACE);
#else
    m_rctBox.setTop(ptButtonLT.y() - INPUT_BOX_HEIGHT);
#endif
    int nOverWidth = ptButtonLT.x() + m_nTotalWidth - PHONE_WIDTH;
    if (nOverWidth > 0)
    {
        m_rctBox.setLeft(PHONE_WIDTH - m_nTotalWidth);
    }
    else
    {
        m_rctBox.setLeft(ptButtonLT.x() - INPUT_BOX_BUTTON_LEFT_BG_EDGE);
    }

    m_rctBox.setWidth(m_nTotalWidth);
#ifndef IF_LYNC
    m_rctBox.setHeight(INPUT_BOX_HEIGHT * nRow + INPUT_BOX_EDGE_WIDTH * 2);
#else
    m_rctBox.setHeight(INPUT_BOX_HEIGHT);
#endif

    setGeometry(m_rctBox);

#ifndef IF_LYNC
    m_rctSelectBox.setRect(m_rctBox.left() - INPUT_BOX_EXTEND_WIDTH
                           , m_rctBox.top() - INPUT_BOX_HEIGHT
                           , m_nTotalWidth + INPUT_BOX_EXTEND_WIDTH + INPUT_BOX_EXTEND_WIDTH
                           , INPUT_BOX_HEIGHT + INPUT_BOX_HEIGHT + INPUT_BOX_HEIGHT);
#endif
    INPUTMETHOD_INFO("CInputBoxButton::SetInputBox, Char Count = %d", m_nCount);
}

int CInputBoxButton::CalculateTextWidth(const QString& strText)
{
    QFont tmpFont;
    tmpFont.setPointSize(INPUT_BOX_CHAR_FONT_SIZE);
    QLabel tmpLabel;
    tmpLabel.setFont(tmpFont);

    //计算文字宽度
    int nStrWidth = tmpLabel.fontMetrics().boundingRect(QRect(0, 0, INPUT_BOX_CHAR_MAX_WIDTH, INPUT_BOX_BUTTON_HEIGHT), Qt::TextWordWrap, strText).width();
#ifndef IF_LYNC
    if (m_nBoxType == INPUT_BOX_VERTICAL_TYPE)
    {
        //如果超过最大限制，则设置为最大限制
        nStrWidth = (nStrWidth > INPUT_BOX_CHAR_VER_MAX_WIDTH) ? INPUT_BOX_CHAR_VER_MAX_WIDTH : nStrWidth;
        //如果小于最小限制，则设置为最小限制
        nStrWidth = (nStrWidth < INPUT_BOX_CHAR_VER_MIN_WIDTH) ? INPUT_BOX_CHAR_VER_MIN_WIDTH : nStrWidth;
    }
    else
    {
        //如果超过最大限制，则设置为最大限制
        nStrWidth = (nStrWidth > INPUT_BOX_CHAR_MAX_WIDTH) ? INPUT_BOX_CHAR_MAX_WIDTH : nStrWidth;
        //如果小于最小限制，则设置为最小限制
        nStrWidth = (nStrWidth < INPUT_BOX_CHAR_MIN_WIDTH) ? INPUT_BOX_CHAR_MIN_WIDTH : nStrWidth;
    }
#else
    //如果超过最大限制，则设置为最大限制
    nStrWidth = (nStrWidth > INPUT_BOX_CHAR_MAX_WIDTH) ? INPUT_BOX_CHAR_MAX_WIDTH : nStrWidth;
    //如果小于最小限制，则设置为最小限制
    nStrWidth = (nStrWidth < INPUT_BOX_CHAR_MIN_WIDTH) ? INPUT_BOX_CHAR_MIN_WIDTH : nStrWidth;
#endif

    return nStrWidth;
}

void CInputBoxButton::paintEvent(QPaintEvent* pe)
{
    if (!m_bIsShow && !m_bNeedFullScreenUpdate)
    {
        return;
    }

    QStylePainter stylePainter(this);

    if (m_nCount <= 0
            || m_vecInputBoxString.size() <= 0
            || m_vecInputBoxString.size() < m_nCount
            || m_vecInputBoxWidth.size() <= 0
            || m_vecInputBoxWidth.size() < m_nCount)
    {
        return;
    }

#ifndef IF_LYNC
    Theme_Draw_9_Pix(stylePainter, INPUT_BOX_BG, rect());
    ////绘制背景色
    //stylePainter.fillRect(rect(), INPUT_BOX_LYNC_BG_COLOR);

    ////绘制边框
    //QRect rctBg = rect();
    //rctBg.setWidth(m_rctBox.width() - 1);
    //rctBg.setHeight(m_rctBox.height() - 1);
    //stylePainter.setPen(INPUT_BOX_LYNC_MARGIN_COLOR);//设置画笔颜色
    //stylePainter.drawRect(rctBg);
#else
    //画左边圆角
    stylePainter.drawPixmap(0, 0, THEME_GET_BMP(INPUT_BOX_BG_LEFT));

    //画中间背景
    QRect rctBGMiddle(INPUT_BOX_BG_FILLET_WIDTH
                      , 0
                      , m_nTotalWidth - INPUT_BOX_BG_FILLET_WIDTH - INPUT_BOX_BG_FILLET_WIDTH
                      , INPUT_BOX_HEIGHT);
    stylePainter.drawPixmap(rctBGMiddle, THEME_GET_BMP(INPUT_BOX_BG_MIDDLE));

    //画右边圆角
    stylePainter.drawPixmap(m_nTotalWidth - INPUT_BOX_BG_FILLET_WIDTH
                            , 0
                            , THEME_GET_BMP(INPUT_BOX_BG_RIGHT));
#endif
    stylePainter.setFont(m_charFont);
    QPen pen = stylePainter.pen();

    //画备选字符
    QRect rctChar;

    int nTotalLeft = 0;
    int nTotalTop = 0;
    for (int i = 0; i < m_nCount; ++i)
    {
        //计算文字位置
        rctChar.setLeft(nTotalLeft + INPUT_BOX_EDGE_WIDTH + INPUT_BOX_HIGHLIGHT_FILLET_WIDTH);
        rctChar.setTop(nTotalTop + INPUT_BOX_EDGE_WIDTH);

        //最后一个要特殊处理
        if (i == m_nCount - 1)
        {
            rctChar.setWidth(m_vecInputBoxWidth[i] - INPUT_BOX_EDGE_WIDTH - INPUT_BOX_EDGE_WIDTH - INPUT_BOX_HIGHLIGHT_FILLET_WIDTH - INPUT_BOX_HIGHLIGHT_FILLET_WIDTH);
        }
        else
        {
            rctChar.setWidth(m_vecInputBoxWidth[i] - INPUT_BOX_EDGE_WIDTH - INPUT_BOX_EDGE_WIDTH - INPUT_BOX_HIGHLIGHT_FILLET_WIDTH - INPUT_BOX_HIGHLIGHT_FILLET_WIDTH);
        }

        rctChar.setHeight(INPUT_BOX_BUTTON_HEIGHT/*- INPUT_BOX_EDGE_WIDTH - INPUT_BOX_EDGE_WIDTH*/);

        //是否高亮文字
        if (m_nIndex == i)
        {
#ifndef IF_LYNC
            stylePainter.fillRect(rctChar, INPUT_BOX_LYNC_LIGHT_BUTTON_COLOR);
#else
            //画左边选中高亮背景圆角
            stylePainter.drawPixmap(nTotalLeft + INPUT_BOX_EDGE_WIDTH
                                    , INPUT_BOX_EDGE_WIDTH
                                    , THEME_GET_BMP(INPUT_BOX_HIGHLIGHT_LEFT));

            //画中间选中高亮背景
            stylePainter.drawPixmap(rctChar, THEME_GET_BMP(INPUT_BOX_HIGHLIGHT_MIDDLE));

            //画右边选中高亮背景圆角
            //最后一个要特殊处理
            if (i == m_nCount - 1)
            {
                stylePainter.drawPixmap(nTotalLeft + m_vecInputBoxWidth[i] - INPUT_BOX_EDGE_WIDTH - INPUT_BOX_HIGHLIGHT_FILLET_WIDTH
                                        , INPUT_BOX_EDGE_WIDTH
                                        , THEME_GET_BMP(INPUT_BOX_HIGHLIGHT_RIGHT));
            }
            else
            {
                stylePainter.drawPixmap(nTotalLeft + m_vecInputBoxWidth[i] - INPUT_BOX_HIGHLIGHT_FILLET_WIDTH
                                        , INPUT_BOX_EDGE_WIDTH
                                        , THEME_GET_BMP(INPUT_BOX_HIGHLIGHT_RIGHT));
            }
#endif
            //设置高亮字体颜色
            pen.setColor(INPUT_BOX_CHAR_COLOR_HL);
            stylePainter.setPen(pen);
        }
        else
        {
            //设置普通状态字体颜色
            pen.setColor(INPUT_BOX_CHAR_COLOR);
            stylePainter.setPen(pen);
        }

        //绘制文字
        QString strDisp = CutOffText(m_vecInputBoxString[i], stylePainter.fontMetrics(), rctChar.width());
        stylePainter.drawText(rctChar, Qt::AlignCenter, strDisp);

#ifndef IF_LYNC
        if (m_nBoxType == INPUT_BOX_VERTICAL_TYPE)
        {
            nTotalTop += INPUT_BOX_BUTTON_HEIGHT;
        }
        else
        {
            if ((i + 1) % (m_nShowMaxNumRow) == 0)
            {
                nTotalLeft = 0;
                nTotalTop += INPUT_BOX_BUTTON_HEIGHT;
            }
            else
            {
                nTotalLeft += m_vecInputBoxWidth[i];
            }
        }
#else
        nTotalLeft += m_vecInputBoxWidth[i];
#endif
    }

    if (m_bNeedFullScreenUpdate)
    {
///     BaseUI_SetFullScreenUpdate(m_bLastFullScreenUpdate);
        m_bNeedFullScreenUpdate = false;
        m_bLastFullScreenUpdate = false;
    }
}

void CInputBoxButton::SetInputBoxHighLight(int nIndex)
{
    if (m_nIndex != nIndex)
    {
        INPUTMETHOD_INFO("CInputBoxButton::SetInputBoxHighLight, High Light Index = %d", nIndex);
        m_nIndex = nIndex;
        update();
    }
}

#ifndef IF_LYNC
bool CInputBoxButton::event(QEvent* pEvent)
{
    if (NULL != pEvent
            && (pEvent->type() == QEvent::MouseButtonPress
                || pEvent->type() == QEvent::MouseButtonRelease))
    {
        QMouseEvent* pMouseEvent = static_cast<QMouseEvent*>(pEvent);
        if (NULL == pMouseEvent)
        {
            return false;
        }

        ProcesseMouseMoveEvent(pMouseEvent);

        return true;
    }

    return QWidget::event(pEvent);
}
#endif

bool CInputBoxButton::ProcesseMouseMoveEvent(QMouseEvent *pMouseEvent)
{
    if (!m_bIsShow || NULL == pMouseEvent)
    {
        return false;
    }

    int nType = pMouseEvent->type();
    QPoint nGlobalPos = pMouseEvent->globalPos();
#ifndef IF_LYNC


    //如果点不在有效位置内，则直接设置为KEYBOARD_INPUTBOX_ERROR_INDEX（无有效选择）
    if (!m_rctBox.contains(nGlobalPos)
            //如果数据不合法也直接返回
            || m_nCount <= 0
            || m_vecInputBoxWidth.size() <= 0
            || m_vecInputBoxWidth.size() < m_nCount)
    {
        SetInputBoxHighLight(KEYBOARD_INPUTBOX_ERROR_INDEX);
        HideInputBox();
        return false;
    }
#endif
    if (nType == QEvent::MouseButtonPress)
    {
        JudgeIndexByMousePosition(nGlobalPos);
    }
    else    if (nType == QEvent::MouseMove)
    {
        //JudgeIndexByMousePosition(nGlobalPos);
    }
    else    if (nType == QEvent::MouseButtonRelease)
    {
        //JudgeIndexByMousePosition(nGlobalPos);
        emit SignalInputBoxPress();
    }

    return true;
}

void CInputBoxButton::JudgeIndexByMousePosition(const QPoint& pt)
{
    if (!m_bIsShow)
    {
        return;
    }

#ifndef IF_LYNC
    QRect rctChar;
    int nTotalLeft = 0;
    int nTotalTop = 0;
    int nSize = m_vecInputBoxWidth.size();
    for (int nIndex = 0; nIndex < nSize; ++nIndex)
    {
        rctChar.setLeft(m_rctBox.left() + nTotalLeft + INPUT_BOX_EDGE_WIDTH);
        rctChar.setTop(m_rctBox.top() + INPUT_BOX_EDGE_WIDTH + nTotalTop);
        rctChar.setWidth(m_vecInputBoxWidth[nIndex]);
        rctChar.setHeight(INPUT_BOX_BUTTON_HEIGHT);

        if (rctChar.contains(pt))
        {
            SetInputBoxHighLight(nIndex);
            break;
        }

        if (m_nBoxType == INPUT_BOX_VERTICAL_TYPE)
        {
            nTotalTop += INPUT_BOX_BUTTON_HEIGHT;
        }
        else
        {
            if ((nIndex + 1) % (m_nShowMaxNumRow) == 0)
            {
                nTotalLeft = 0;
                nTotalTop += INPUT_BOX_BUTTON_HEIGHT;
            }
            else
            {
                nTotalLeft += m_vecInputBoxWidth[nIndex];
            }
        }
    }
#else
    int nSplitX = 0;
    int nPointX = pt.x();

    if (m_nCount <= 0)
    {
        return;
    }
    else if (1 == m_nCount)
    {
        SetInputBoxHighLight(0);
    }
    else if (2 == m_nCount)
    {
        nSplitX = m_rctBox.left() + m_vecInputBoxWidth[0];
        if (nPointX <= nSplitX + INPUT_BOX_EDGE_WIDTH_HALF)
        {
            SetInputBoxHighLight(0);
        }
        else
        {
            SetInputBoxHighLight(1);
        }
    }
    else
    {
        bool bResult = false;

        nSplitX = m_rctBox.left();
        for (int nIndex = 0; nIndex < m_nCount; ++nIndex)
        {
            nSplitX += m_vecInputBoxWidth[nIndex];
            if (nPointX <= nSplitX + INPUT_BOX_EDGE_WIDTH_HALF)
            {
                SetInputBoxHighLight(nIndex);
                bResult = true;
                break;
            }
        }

        if (!bResult)
        {
            SetInputBoxHighLight(m_nCount - 1);
        }
    }
#endif
}
