#include "messagelistitem.h"
#include "baseui/fontcolor.h"
#include "interfacedefine.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/ilistitemdelegate.h"
#include "baseui/t4xpicpath.h"

namespace
{
#define SETTING_WIGET_DEFAUT_X 240  //内容控件相对item的x坐标
#define DEFAULT_MENU_ENTER_POS_RIGHT 36
#define DEFAULT_MENU_ENTER_POS_WIDTH 13
#define DEFAULT_MENU_ENTER_POS_TOP 15
#define DEFAULT_MENU_ENTER_POS_HEIGHT 17
#define MENU_ICON_HEIGHT 26
#define MENU_ICON_WIDTH 29
#define SETTING_ITEM_WGT_WIDTH 223
#define DEFAULT_COLUMN_SAPCING 8
#define DEFAULT_ICON_LABEL_SAPCING 5
#define SLIDER_BAR_WIDTH 4 //53
#define MENU_SWITCH_WGT_WIDTH 200

#define DISPLAY_LABEL_TOP   4
#define DISPLAY_LABEL_HEIGHT    20

#define DISPLAY_TIME_TOP    24
#define DISPLAY_TIME_HEIGHT 20

#define DISPLAY_MARGIN_BETWENN 1

//  const QColor SETTING_ITEM_PRESS_COLOR = THEME_LIST_FOCUS_TEXT_COLOR;
//  const QColor SETTING_ITEM_SELECT_COLOR = THEME_LIST_FOCUS_TEXT_COLOR;
//  const QColor SETTING_ITEM_NORMAL_COLOR = SELECT_BY_T46ANDT29(Qt::black, Qt::white);

const QColor SETTING_ITEM_NORMAL_COLOR = Qt::black;
const QColor SETTING_MESSAGE_ITEM_NORMAL_COLOR = Qt::gray;

}

CMessageListItem::CMessageListItem(QWidget * parent, QString strIcon, QString strLabel,
                                   QString strAction /*= ""*/, bool bAddIcon/* = false*/
                                   , int iDataIndex/* = -1*/, bool bAddInex/* = true*/, bool bWithStatus/* = false*/)
    : CSettingItem(parent)
    , m_btnEnter(this)
    , m_bAddIcon(bAddIcon)
    , m_bWithStatus(bWithStatus)
{
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
    // 设置是否有选中效果
    SetChangeStyleOnSelect(true);
#endif
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);
#endif

    m_nType |= LIST_ITEM_SUB_TYPE_MESSAGE_LIST;

    m_bAddIndex = bAddInex;
    m_pWgt = NULL;
    if (m_bAddIcon)
    {
        yl::string strIconPath = strIcon.toUtf8().data();
        m_iconMap = THEME_GET_BMP(strIconPath.c_str());
    }
    SetLabText(strLabel);
    m_strAction = strAction;
    SetDataIndex(iDataIndex);
    GenerateLabelText();

    Relayout();
    setObjectName("CMessageListItem");
}

CMessageListItem::CMessageListItem(QWidget * pParent, QWidgetPair & pair)
    : CSettingItem(pParent, pair.first, pair.second, pair.third, pair.iIndex, pair.bAddIndex, false)
    , m_btnEnter(this)
    , m_bAddIcon(pair.bAddIcon)
    , m_bWithStatus(pair.bWithStatus)
{
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
    // 设置是否有选中效果
    SetChangeStyleOnSelect(false);
#endif
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);

    SetClickBg(PIC_GENERAL_LIST_ITEM_CLICK_BG);
#endif


    m_nType |= LIST_ITEM_SUB_TYPE_MESSAGE_LIST;

    if (m_bAddIcon)
    {
        yl::string strIconPath = pair.strIcon;
        m_iconMap = THEME_GET_BMP(strIconPath.c_str());
    }
    Relayout();
    setObjectName("CMessageListItem");
}

void CMessageListItem::AddIcon(LPCSTR strIconPath)
{
    if (strcmp(strIconPath, "") == 0)
    {
        return;
    }

    m_iconMap = THEME_GET_BMP(strIconPath);
    if (m_iconMap.isNull())
    {
        return;
    }

    m_bAddIcon = true;
    Relayout();
}

CMessageListItem::~CMessageListItem()
{
    //
}

void CMessageListItem::ApplyNomalStyle()
{
    QPalette pal;

    const QColor colorPress = SETTING_MESSAGE_ITEM_NORMAL_COLOR;

    if (NULL != m_pWgt)
    {
        pal = m_pWgt->palette();
        pal.setBrush(QPalette::Text, QBrush(colorPress));
        pal.setBrush(QPalette::WindowText, QBrush(colorPress));
        m_pWgt->setPalette(pal);
    }

    const QColor color = SETTING_ITEM_NORMAL_COLOR;

    if (NULL != m_pLabDisplay)
    {
        pal = m_pLabDisplay->palette();
        pal.setBrush(QPalette::Foreground, QBrush(color));
        m_pLabDisplay->setPalette(pal);
    }
}

int CMessageListItem::GetMsgListItemType()
{
    return (LIST_ITEM_TYPE_SETTING | LIST_ITEM_SUB_TYPE_MESSAGE_LIST);
}

void CMessageListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
//  PaintFocusBg(stylePainter);
#ifdef  QT_TOUCH_SUPPORT
    PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());
#endif
    ApplyStyleByState(GetState());

    int nLeftBegin = 0;
    int nRightEnd = 0;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }
    if (m_bAddIcon && !m_iconMap.isNull())
    {
        QSize iconSize = m_iconMap.size();
        stylePainter.drawPixmap(nLeftBegin, (rect().height() - MENU_ICON_HEIGHT) / 2
                                , iconSize.width() < MENU_ICON_WIDTH ? iconSize.width() : MENU_ICON_WIDTH
                                , iconSize.height() < MENU_ICON_HEIGHT ? iconSize.height() : MENU_ICON_HEIGHT, m_iconMap);
    }

    m_btnEnter.PaintButton(stylePainter, PIC_GENERAL_LIST_ITEM_ENTER,
                           PIC_GENERAL_LIST_ITEM_ENTER_CLICK, PIC_GENERAL_LIST_ITEM_ENTER);
}

void CMessageListItem::Relayout()
{
    QRect rcItem = rect();

    int nLeftBegin = 0;
    int nRightEnd = 0;
    int nbtnEnterLeft = rcItem.right() - DEFAULT_MENU_ENTER_POS_RIGHT - DEFAULT_MENU_ENTER_POS_WIDTH;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }

    if (m_bAddIcon && (!m_iconMap.isNull()))
    {
        nLeftBegin += MENU_ICON_WIDTH + DEFAULT_ICON_LABEL_SAPCING;
    }

    m_pLabDisplay->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    if (NULL != m_pWgt)
    {
        if (m_pLabDisplay)
        {
            m_pLabDisplay->setGeometry(nLeftBegin, DISPLAY_LABEL_TOP + DISPLAY_MARGIN_BETWENN
                                       , SETTING_WIGET_DEFAUT_X - DEFAULT_COLUMN_SAPCING - nLeftBegin, DISPLAY_LABEL_HEIGHT);
        }

        if (m_bWithStatus && m_pWgt->inherits("QLabel"))
        {
            ((QLabel *)m_pWgt)->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        }
//      int nLeft = m_bWithStatus ? nbtnEnterLeft - MENU_SWITCH_ARROW_SPACINE - MENU_SWITCH_WGT_WIDTH : SETTING_WIGET_DEFAUT_X;
#if IF_SETTING_ITEM_WGT_HEIGHT_FIXED
//      int nTop = (rcItem.height() - m_pWgt->height()) / 2;
        QRect rcWgt(nLeftBegin, DISPLAY_TIME_TOP - DISPLAY_MARGIN_BETWENN,
                    m_bWithStatus ? MENU_SWITCH_WGT_WIDTH : SETTING_ITEM_WGT_WIDTH - SLIDER_BAR_WIDTH
                    , DISPLAY_TIME_HEIGHT);
#else
        QRect rcWgt(nLeft, 0, m_bWithStatus ? MENU_SWITCH_WGT_WIDTH : SETTING_ITEM_WGT_WIDTH -
                    SLIDER_BAR_WIDTH
                    , this->height());
#endif
        m_pWgt->setGeometry(rcWgt);
    }
    else
    {
        if (m_pLabDisplay)
        {
            m_pLabDisplay->setGeometry(nLeftBegin, DISPLAY_LABEL_TOP
                                       , nbtnEnterLeft - DEFAULT_COLUMN_SAPCING - nLeftBegin, DISPLAY_LABEL_HEIGHT);
        }
    }


    int nTop = rcItem.top() + DEFAULT_MENU_ENTER_POS_TOP;
    QRect rcBtnEnter(nbtnEnterLeft, nTop, DEFAULT_MENU_ENTER_POS_WIDTH, DEFAULT_MENU_ENTER_POS_HEIGHT);

    m_btnEnter.SetRect(rcBtnEnter);

}
