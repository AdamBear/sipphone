#include "optionboxitem.h"
#include "baseui/fontcolor.h"

#include "imagemanager/modimagemanager.h"
#include "baseui/t4xpicpath.h"

namespace
{
const int OPTIONBOX_ITEM_BG_WIDTH(357);
const int OPTIONBOX_ITEM_BG_HEIGHT(48);

const int OPTIONBOX_BG_EDGE_WIDTH(0);
const int OPTIONBOX_ITEM_EDGE_MARGIN(5);

const int OPTIONBOX_FOCUS_ICON_MARGIN(18);
const int OPTIONBOX_FOCUS_ICON_RIGHT_MARGIN(12);
const int OPTIONBOX_FOCUS_ICON_WIDTH(18);

const static QRect OPTIONBOX_SELECTED_RECT(OPTIONBOX_FOCUS_ICON_MARGIN, OPTIONBOX_FOCUS_ICON_MARGIN,
        OPTIONBOX_FOCUS_ICON_WIDTH, OPTIONBOX_FOCUS_ICON_WIDTH);
}

COptionBoxItem::COptionBoxItem(QWidget * parent, bool bWithFocos)
    : CListItem(LIST_ITEM_TYPE_OPTION_BOX, parent, NULL)
    , m_pLabelText(NULL)
    , m_bWithFocus(bWithFocos)
    , m_Selected(false)
{

    setObjectName("COptionBoxItem");

    setGeometry(0, 0, 0, OPTIONBOX_ITEM_BG_HEIGHT);

    m_pLabelText = new QLabel(this);
    if (NULL != m_pLabelText)
    {
        m_pLabelText->show();
        m_pLabelText->setObjectName("LabelText");
        QPalette p = m_pLabelText->palette();
        p.setBrush(QPalette::WindowText, Qt::black);
        m_pLabelText->setPalette(p);

        m_pLabelText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
}

COptionBoxItem::~COptionBoxItem()
{
}

void COptionBoxItem::SetText(const QString & strText)
{
    if (NULL != m_pLabelText)
    {
        m_pLabelText->setText(strText);
    }
}


void COptionBoxItem::SetItemRightMargin(int nRightMargin)
{
    m_rectBG = QRect(OPTIONBOX_BG_EDGE_WIDTH, OPTIONBOX_BG_EDGE_WIDTH,
                     OPTIONBOX_ITEM_BG_WIDTH - nRightMargin, OPTIONBOX_ITEM_BG_HEIGHT);

    if (NULL != m_pLabelText)
    {
        if (m_bWithFocus)
        {
            QRect OPTIONBOX_WIDTH_FOCUS_TEXT_RECT(
                OPTIONBOX_FOCUS_ICON_MARGIN + OPTIONBOX_FOCUS_ICON_WIDTH + OPTIONBOX_FOCUS_ICON_RIGHT_MARGIN
                , 0
                , OPTIONBOX_ITEM_BG_WIDTH - nRightMargin - OPTIONBOX_FOCUS_ICON_MARGIN - OPTIONBOX_FOCUS_ICON_WIDTH
                - OPTIONBOX_FOCUS_ICON_RIGHT_MARGIN - OPTIONBOX_ITEM_EDGE_MARGIN
                , OPTIONBOX_ITEM_BG_HEIGHT);
            m_pLabelText->setGeometry(OPTIONBOX_WIDTH_FOCUS_TEXT_RECT);
        }
        else
        {
            QRect OPTIONBOX_TEXT_RECT(
                OPTIONBOX_ITEM_EDGE_MARGIN
                , 0
                , OPTIONBOX_ITEM_BG_WIDTH - nRightMargin - OPTIONBOX_ITEM_EDGE_MARGIN - OPTIONBOX_ITEM_EDGE_MARGIN
                , OPTIONBOX_ITEM_BG_HEIGHT);
            m_pLabelText->setGeometry(OPTIONBOX_TEXT_RECT);
        }
    }
}

void COptionBoxItem::SetCurrentSelected(bool bSelected)
{
    m_Selected = bSelected;
}

void COptionBoxItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    if (m_bWithFocus)
    {
        if (m_Selected)
        {
            stylePainter.drawPixmap(OPTIONBOX_SELECTED_RECT, THEME_GET_BMP(PIC_MSGBOX_OPTION_BOX_SELECTED));
        }
        else
        {
            stylePainter.drawPixmap(OPTIONBOX_SELECTED_RECT, THEME_GET_BMP(PIC_MSGBOX_OPTION_BOX_NORMAL));
        }
    }

    CListItem::paintEvent(pEvent);
}
