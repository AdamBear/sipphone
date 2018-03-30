#include "wirelesssearchitem.h"
#include "notelabel.h"
#include "imagemanager/modimagemanager.h"
#include "baseui/t4xpicpath.h"
#include "baseui/framelistaction.h"
#include "uikernel/framelistbase.h"
#include <QStylePainter>

namespace
{
//#define SELECT_TYPE_T49

#ifdef SELECT_TYPE_T49
#define SETTING_ITEM_HEIGHT 86  //item的高度
#define SETTING_ITEM_LABEL_HEIGHT 58
#else
#define SETTING_ITEM_HEIGHT 47  //item的高度
#define SETTING_ITEM_LABEL_HEIGHT 24

// #define SEARCH_ITEM_LEFT            20
#define SEARCH_ITEM_WIDTH           473
#endif

// wireless search and statistics background rect
const QRect RECT_WIRELESS_SEARCH_STATISTICS_BACKGROUND(2, 0, 836, 86);
// wireless search and statistics rect
const QRect RECT_WIRELESS_SEARCH_STATISTICS_NOTE(32, 14, 800, SETTING_ITEM_LABEL_HEIGHT);
}

Q_DECLARE_METATYPE(WirelessSearchStatisticsData);

WirelessSearchStatisticsItem::WirelessSearchStatisticsItem(QWidget * parent,
        int iDataIndex/* = -1*/, bool bAddInex/* = false*/)
    : CSettingItem(parent)
    , m_pLblNote(NULL)
{
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);
#endif

    m_nType |= LIST_ITEM_SUB_TYPE_WIRELESS_SEARCH;

    m_bAddIndex = bAddInex;
    m_pWgt = NULL;
    SetDataIndex(iDataIndex);
    GenerateLabelText();

    m_pLblNote = new NoteLabel(this);
    if (NULL != m_pLblNote)
    {
        m_pLblNote->SetLabelType(INT_FOREVER);
        m_pLblNote->SetLabelText("");
    }
}

WirelessSearchStatisticsItem::WirelessSearchStatisticsItem(QWidget * pParent, QWidgetPair & pair)
    : CSettingItem(pParent, pair.first, pair.second, pair.third, pair.iIndex, false)
    , m_pLblNote(NULL)
{
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);
#endif

    m_nType |= LIST_ITEM_SUB_TYPE_WIRELESS_SEARCH;

    m_ItemData = pair.appendVariantData.value<WirelessSearchStatisticsData>();
    m_pLblNote = new NoteLabel(this);
    if (NULL != m_pLblNote)
    {
        m_pLblNote->SetLabelType(m_ItemData.s_eType);
        m_pLblNote->SetLabelText(m_ItemData.s_strInfo);
    }
}

WirelessSearchStatisticsItem::~WirelessSearchStatisticsItem()
{
    //
}

int WirelessSearchStatisticsItem::GetWirelessSearchStatisticsItemType()
{
    return (LIST_ITEM_TYPE_SETTING | LIST_ITEM_SUB_TYPE_WIRELESS_SEARCH);
}

void WirelessSearchStatisticsItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
#ifdef  QT_TOUCH_SUPPORT
    //  PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());
    //  ApplyStyleByState(GetState());
#endif

    QPixmap pmIcon = THEME_GET_BMP("");
    if (!pmIcon.isNull())
    {
        stylePainter.drawPixmap(RECT_WIRELESS_SEARCH_STATISTICS_BACKGROUND, pmIcon);
    }
}

void WirelessSearchStatisticsItem::Relayout()
{
    int nLeftBegin = 0;
    int nRightEnd = 0;

#ifdef FL_SUPPORT_CUSTOM_ITEM_STYLE
    if (NULL != m_pStyle)
    {
        //m_pStyle->GetContentPos(nLeftBegin, nRightEnd);
    }
#endif

#ifdef SELECT_TYPE_T49
    if (NULL != m_pLblNote)
    {
        m_pLblNote->setGeometry(RECT_WIRELESS_SEARCH_STATISTICS_NOTE);
    }
#else
    QRect rcItem = rect();
    QPoint ptBegin(rcItem.left(), rcItem.top());
    if (NULL != m_pDelegate)
    {
        int nLeftMargin = 0;
        int nRightMargin = 0;

        m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);

        ptBegin.setX(ptBegin.x() + nLeftMargin);
    }
    if (m_pLblNote)
    {
        m_pLblNote->setGeometry(ptBegin.x()//SEARCH_ITEM_LEFT
                                , (SETTING_ITEM_HEIGHT - m_pLblNote->height()) / 2
                                , SEARCH_ITEM_WIDTH
                                , SETTING_ITEM_LABEL_HEIGHT);
    }
#endif
}

void WirelessSearchStatisticsItem::SetDisplayAndType(const QString & strDisplay,
        Item_Note_Type eType)
{
    m_ItemData.s_eType = eType;
    m_ItemData.s_strInfo = strDisplay;
    if (NULL != m_pLblNote)
    {
        m_pLblNote->SetLabelType(eType);
        m_pLblNote->SetLabelText(strDisplay);
        m_pLblNote->show();
    }
}

void WirelessSearchStatisticsItem::SetItemData(QWidgetPair qPair)
{
    m_ItemData = qPair.appendVariantData.value<WirelessSearchStatisticsData>();
    SETTINGUI_INFO("[WirelessSearchStatisticsItem::SetItemData] text[%s], Type[%d]"
                   , m_ItemData.s_strInfo.toUtf8().data(), m_ItemData.s_eType);
    if (NULL != m_pLblNote)
    {
        m_pLblNote->SetLabelType(m_ItemData.s_eType);
        m_pLblNote->SetLabelText(m_ItemData.s_strInfo);
    }
    update();
}
