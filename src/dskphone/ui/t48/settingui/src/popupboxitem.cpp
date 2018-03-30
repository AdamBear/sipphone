#include "popupboxitem.h"
#include "uikernel/qcheckbox.h"
#include "baseui/t4xpicpath.h"
//#include "imagecachemanager/modimagecachemanager.h"
#include "imagemanager/modimagemanager.h"
#include "settingui/src/bluetoothuicommon.h"
//#include "settingui/include/bluetoothcommondata.h"
#include <QStylePainter>

namespace
{
#ifdef _T49
#define POPUP_ITEM_HEIGHT               74

#define WIDGET_LEFT_START_POS_X         284//324
#define DISPLAY_LEFT_START_POX_X        30
#define DISPLAY_LEFT_START_POX_Y        0
#define DISPLAY_LEFT_WIDTH              250//290
#define DISPLAY_LEFT_HEIGHT             74
// the size of font
#define DEFAULT_ITEM_TEXT_FONT_SIZE     23

// background rect
const QRect RECT_POPUP_BLUETOOTH_BACKGROUND(0, 0, 564, POPUP_ITEM_HEIGHT + 2);
#else
#define POPUP_ITEM_HEIGHT               48

#define WIDGET_LEFT_START_POS_X         170
#define DISPLAY_LEFT_START_POX_X        7
#define DISPLAY_LEFT_START_POX_Y        0
#define DISPLAY_LEFT_WIDTH              160
#define DISPLAY_LEFT_HEIGHT             48
// the size of font
#define DEFAULT_ITEM_TEXT_FONT_SIZE     16

// background rect
const QRect RECT_POPUP_BLUETOOTH_BACKGROUND(1, 0, 355, POPUP_ITEM_HEIGHT + 2);
#endif

#define DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR QColor(0, 0, 0)
}

PopupBoxItem::PopupBoxItem(QWidget * parent /*= NULL*/, IListItemAction * pAction /*= NULL*/)
    : CListItem(LIST_ITEM_TYPE_POPUP_BLUETOOOTH, parent, pAction)
    , m_pWidget(NULL)
    , m_strLeftDisplay("")
    , m_iWidgetStartX(WIDGET_LEFT_START_POS_X)
    , m_rectLeftDisplay(DISPLAY_LEFT_START_POX_X, DISPLAY_LEFT_START_POX_Y
                        , DISPLAY_LEFT_WIDTH, DISPLAY_LEFT_HEIGHT)
{
    setObjectName("PopupBoxItem");
}

PopupBoxItem::~PopupBoxItem()
{
}

void PopupBoxItem::SetLeftDisplay(const QString & strDisplay)
{
    if (!strDisplay.isNull())
    {
        m_strLeftDisplay = strDisplay;
        update();
    }
}

QString PopupBoxItem::GetLeftDisplay()
{
    return m_strLeftDisplay;
}

void PopupBoxItem::SetLeftDisplayRect(const QRect rectDisplay)
{
    m_rectLeftDisplay = rectDisplay;
}

void PopupBoxItem::SetWidget(QWidget * pWgt)
{
    if (NULL != pWgt)
    {
        m_pWidget = pWgt;
        m_pWidget->setParent(this);
        m_pWidget->installEventFilter(this);
        if (m_pWidget->inherits("qCheckBox"))
        {
            qCheckBox * pChk = static_cast<qCheckBox *>(m_pWidget);
            if (NULL != pChk)
            {
                connect(pChk->GetRadioButtonOn(), SIGNAL(toggled(bool)), this, SLOT(OnCheckBoxChanged(bool)));
            }
        }
    }
}

QWidget * PopupBoxItem::GetWidget()
{
    return m_pWidget;
}

void PopupBoxItem::SetWidgetLeftStartX(const int iStartX)
{
    m_iWidgetStartX = iStartX < 0 ? 0 : iStartX;
}

void PopupBoxItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    // draw background
    if (IsDown() && NULL == m_pWidget)
    {
        QPixmap pmIcon = THEME_GET_BMP(PIC_MSGBOX_SELECT_BOX_ITEM_SELECT);
        if (!pmIcon.isNull())
        {
            stylePainter.drawPixmap(RECT_POPUP_BLUETOOTH_BACKGROUND, pmIcon);
        }
    }
    // draw text
    QRect rc = m_rectLeftDisplay;
    if (NULL == m_pWidget)
    {
        rc.setWidth(rc.width() + 104);
    }
    PaintInfo(stylePainter, rc);
}

void PopupBoxItem::PaintInfo(QStylePainter & stylePainter, const QRect & rcDisplay,
                             int nAlign /*= Qt::AlignVCenter*/)
{
    bool bEnable = isEnabled();
    const QPalette & txtPalette = palette();
    QFont fontText = stylePainter.font();

    stylePainter.setPen(DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR);

    if (!m_strLeftDisplay.isEmpty())
    {
        fontText.setPointSize(DEFAULT_ITEM_TEXT_FONT_SIZE);
        fontText.setWeight(QFont::Bold);
        fontText.setBold(false);
        stylePainter.setFont(fontText);

        stylePainter.drawItemText(m_rectLeftDisplay, (Qt::AlignLeft | nAlign), txtPalette, bEnable,
                                  m_strLeftDisplay);
    }
}

void PopupBoxItem::Relayout()
{
    if (NULL != m_pWidget)
    {
        int iX = WIDGET_LEFT_START_POS_X;
        int iY = (POPUP_ITEM_HEIGHT - m_pWidget->height()) >> 1;
        QRect rc(iX, iY, m_pWidget->width(), height());

        m_pWidget->setGeometry(rc);
        m_pWidget->raise();
    }
}

void PopupBoxItem::OnCheckBoxChanged(bool bOn)
{
    if (NULL != m_pWidget)
    {
        DoAction(LIST_ITEM_ACTION_BLUETOOTH_MOBILE_SYNC);
    }
}

