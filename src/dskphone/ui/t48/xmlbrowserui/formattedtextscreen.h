#ifndef _FORMATTED_TEXT_
#define _FORMATTED_TEXT_

#include <QtGui>

#include "baselabel.h"
#include "cuiregionbase.h"

#include "uikernel/scrollpage.h"

//数据定义
//字号大小
enum FONT_SIZE
{
    FONT_SIZE_SMALL     = 0,
    FONT_SIZE_NORMAL,
    FONT_SIZE_DOULBLE,
    FONT_SIZE_LARGE,
};

//文字颜色
enum TEXT_COLOR
{
    TEXT_COLOR_WHITE        = 0,
    TEXT_COLOR_BLACK,
    TEXT_COLOR_RED,
    TEXT_COLOR_BLUE,
    TEXT_COLOR_GRAY,
};

//文字位置
enum TEXT_ALIGNMENT
{
    TEXT_ALIGNMENT_LEFT     = 0,
    TEXT_ALIGNMENT_CENTER,
    TEXT_ALIGNMENT_RIGHT,
};

struct FormattedTextItemData
{
    TEXT_FONT_SIZE      m_eFontSize;        // 字体大小
    TEXT_FONT_ALIGN     m_eAlign;           // 对齐方式
    TEXT_FONT_COLOR     m_eTextColor;       // 字体颜色
    QString     m_strText;  // 文本内容

    FormattedTextItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_eFontSize = TEXT_FONT_SIZE_NORMAL;
        m_strText = "";
        m_eTextColor = TEXT_FONT_COLOR_WHITE;
        m_eAlign = TEXT_FONT_ALIGN_LEFT;
    }
};

//////////////////////////////////////////////////
class CT3XScrollPage;
class AutoResizeListView;

class CDlgFormattedTextScreen : public CUIRegionBase
{
    Q_OBJECT

public:
    CDlgFormattedTextScreen(QWidget * parent = 0, Qt::WFlags flags = 0);
    ~CDlgFormattedTextScreen();

    //必须实现的函数
    void Update(void * pData);

    bool eventFilter(QObject * pObj, QEvent * pEvent);

private:
    static const int MAX_HEADER = 2;    // 最大的Header的行数
    static const int MAX_FOOTER = 1;    // 最大的Footer的行数

    AutoResizeListView * m_HeaderView;  // 显示Header的label
    AutoResizeListView * m_FooterView;  // 显示Footer的label
    AutoResizeListView * m_TextView;    // 显示文本的label
    CScrollPage    *    m_pScrollPage;
};


#endif
