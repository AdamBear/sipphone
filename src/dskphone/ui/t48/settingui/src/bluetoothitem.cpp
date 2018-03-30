#include "bluetoothitem.h"
#include <QStylePainter>
#include "uikernel/ilistitemdelegate.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "settingui/src/bluetoothuicommon.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"
#include <QTimer>

//****************************bluetooth defiene begin
// #define DEFAULT_MENU_ENTER_POS_RIGHT 36
// #define DEFAULT_MENU_ENTER_POS_WIDTH 13
#define SETTING_ITEM_HEIGHT 47  //item的高度
#define SETTING_ITEM_LABEL_HEIGHT 24

// #define DEFAULT_SIMPLE_INFO_ITEM_HEIGHT 55

#define DEFAULT_INFO_POS_ICON_LEFT 0
#define DEFALUT_INFO_POS_ICON_TOP 8
#define DEFAULT_INFO_POS_ICON_WIDTH 30
#define DEFAULT_INFO_POS_ICON_HEIGHT 30

#define DEFALUT_INFO_POS_TEXT_LEFT_MARGIN 10
#define DEFAULT_INFO_POS_TITLE_TOP 6
#define DEFAULT_INFO_POS_TITLE_HEIGHT 30
#define DEFAULT_INFO_POS_INFO_BOTTOM 47
#define DEFAULT_INFO_POS_INFO_HEIGHT 30

//bluetooth Detail
#define DEFAULT_CONTACT_POS_ICON_DETAIL_X_OFFSET 50
#define DEFAULT_CONTACT_POS_ICON_DETAIL_TOP 8
#define DEFAULT_CONTACT_POS_ICON_DETAIL_WIDTH 30
#define DEFAULT_CONTACT_POS_ICON_DETAIL_HEIGHT 30

// bluetooth device nums background
const QRect RECT_BLUETOOTH_DEVICE_NUM_BG(14, 5, 500, 39);
#define DEFAULT_SIMPLE_INFO_SELECT_TEXT_COLOR QColor(255, 255, 255)
#define DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR QColor(0, 0, 0)

#define TIME_CLOCK_OUT          2000

#ifdef SUPPORT_PHONE_DEVICE
// 基本图标、连接成功图标、自动连接图标、自动连接成功图标
LPCSTR g_pBluetoothIcon[] = {PIC_BLUE_ICON, PIC_BLUE_ICON_WHITE, PIC_BLUE_AUTO_ICON, PIC_BLUE_AUTO_ICON_WHITE};
// 手机基本图标、手机连接成功图标、手机自动连接图标、手机自动连接成功图标
LPCSTR g_pMobileBluetoothIcon[] = {PIC_BLUE_MOBILE_ICON, PIC_BLUE_MOBILE_ICON_WHITE, PIC_BLUE_AUTO_MOBILE_ICON, PIC_BLUE_AUTO_MOBILE_ICON_WHITE};
#endif

//****************************bluetooth defiene end
Q_DECLARE_METATYPE(BluetoothAddData);

/*****************************************Class  CSettingBluetoothDeviceListItem***************************************/
CSettingBluetoothDeviceListItem::CSettingBluetoothDeviceListItem(QWidget * parent,
        int iDataIndex/* = -1*/, bool bAddInex/* = false*/)
    : CSettingItem(parent)
{
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);
#endif

    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_BLUETOOTHDEVICELIST;

    m_bAddIndex = bAddInex;
    m_pWgt = NULL;
    SetDataIndex(iDataIndex);
    GenerateLabelText();

}

CSettingBluetoothDeviceListItem::CSettingBluetoothDeviceListItem(QWidget * pParent,
        QWidgetPair & pair)
    : CSettingItem(pParent, pair.first, pair.second, pair.third, pair.iIndex, false)
{
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);
#endif

    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_BLUETOOTHDEVICELIST;

}

CSettingBluetoothDeviceListItem::~CSettingBluetoothDeviceListItem()
{
}

int CSettingBluetoothDeviceListItem::GetSettingBluetoothDeviceListItemType()
{
    return (LIST_ITEM_TYPE_SETTING | LIST_ITEM_SUB_TYPE_SETTING_BLUETOOTHDEVICELIST);
}

void CSettingBluetoothDeviceListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
#ifdef  QT_TOUCH_SUPPORT
    PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());
    ApplyStyleByState(GetState());
#endif
    int nLeftBegin = 0;
    int nRightEnd = 0;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }
}

void CSettingBluetoothDeviceListItem::Relayout()
{
    int nLeftBegin = 0;
    int nRightEnd = 0;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }

    if (m_pLabDisplay)
    {
        m_pLabDisplay->setGeometry(nLeftBegin + 6, (SETTING_ITEM_HEIGHT - SETTING_ITEM_LABEL_HEIGHT) / 2
                                   , 460, SETTING_ITEM_LABEL_HEIGHT);
    }
}


/*****************************************Class  CSettingBluetoothItem***************************************/
CSettingBluetoothItem::CSettingBluetoothItem(QWidget * parent, QString strDisplayName,
        QString strAction, yl::string strMac, yl::string strNote, yl::string strIconPath,
        QString strInfo, int index /* = -1 */, bool bAddInex /* = false */, bool bAddNote /* = false */)
    : CSettingItem(parent, strDisplayName, NULL, strAction, index, bAddInex)
    , m_bAddNote(bAddNote)
    , m_strIconPath(strIconPath)
    , m_strInfo(strInfo)
    , m_btnDetail(this)
#ifdef BLUETOOTH_USED_NEW_VERSION
    , m_pTimer(NULL)
#endif
#ifdef IF_ENABLE_TESTABILITY
    , m_rcIcon(QRect())
    , m_rcLblMac(QRect())
    , m_rcLblDis(QRect())
    , m_rcLblNote(QRect())
#endif
{
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);
#endif

    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_BLUETOOTH;
    m_bluetoothData.strMac = strMac;
    m_bluetoothData.strNote = strNote;

#ifdef BLUETOOTH_USED_NEW_VERSION
    m_pTimer = new QTimer(this);
    if (NULL != m_pTimer)
    {
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(RemoveNote()));
    }
#endif
}

CSettingBluetoothItem::CSettingBluetoothItem(QWidget * pParent, QWidgetPair & pair,
        bool bAddIndex/* = true*/)
    : CSettingItem(pParent, pair.first, pair.second, pair.third, pair.iIndex, false)
    , m_bAddNote(false)
    , m_strIconPath(pair.strIcon)
    , m_btnDetail(this)
#ifdef BLUETOOTH_USED_NEW_VERSION
    , m_pTimer(NULL)
#endif
#ifdef IF_ENABLE_TESTABILITY
    , m_rcIcon(QRect())
    , m_rcLblMac(QRect())
    , m_rcLblDis(QRect())
    , m_rcLblNote(QRect())
#endif
{
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);
#endif

    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_BLUETOOTH;
    m_bluetoothData = pair.appendVariantData.value<BluetoothAddData>();
    m_strInfo = m_bluetoothData.strInfo;

#ifdef BLUETOOTH_USED_NEW_VERSION
    m_pTimer = new QTimer(this);
    if (NULL != m_pTimer)
    {
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(RemoveNote()));
    }
#endif
}

CSettingBluetoothItem::~CSettingBluetoothItem()
{
#ifdef BLUETOOTH_USED_NEW_VERSION
    StopTimeClock();
#endif
}

void CSettingBluetoothItem::RemoveNote()
{
#ifdef BLUETOOTH_USED_NEW_VERSION
    StopTimeClock();
#endif
    SETTINGUI_INFO("CSettingBluetoothItem::RemoveNote() removeNote[%s] ",
                   m_bluetoothData.strNote.c_str());
    m_bluetoothData.strNote.clear();
    this->update();
}

int CSettingBluetoothItem::GetSettingBluetoothItemType()
{
    return (LIST_ITEM_TYPE_SETTING | LIST_ITEM_SUB_TYPE_SETTING_BLUETOOTH);
}

void CSettingBluetoothItem::paintEvent(QPaintEvent * pEvent)
{
    SETTINGUI_INFO("CSettingBluetoothItem::paintEvent paint ");
    QStylePainter stylePainter(this);
#ifdef  QT_TOUCH_SUPPORT
    PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());
    ApplyStyleByState(GetState());
#endif

    QRect rcItem = rect();
    QPoint ptBegin(rcItem.left(), rcItem.top());

    if (NULL != m_pDelegate)
    {
        int nLeftMargin = 0;
        int nRightMargin = 0;

        m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);

        ptBegin.setX(ptBegin.x() + nLeftMargin);
    }

    PaintDefault(stylePainter, ptBegin);
}

void CSettingBluetoothItem::Relayout()
{
    QRect rcItem = rect();

    int nLeft = rcItem.right() - DEFAULT_CONTACT_POS_ICON_DETAIL_X_OFFSET;

    m_btnDetail.SetRect(nLeft, DEFAULT_CONTACT_POS_ICON_DETAIL_TOP,
                        DEFAULT_CONTACT_POS_ICON_DETAIL_WIDTH, DEFAULT_CONTACT_POS_ICON_DETAIL_HEIGHT);
}

void CSettingBluetoothItem::PaintDefault(QStylePainter & stylePainter, const QPoint & ptBegin,
        int nInfoWidth/* = 0*/, const char * pszDefaultIcon/* = NULL*/)
{
    QPoint ptIcon(ptBegin.x() + DEFAULT_INFO_POS_ICON_LEFT, ptBegin.y());

    PaintIconInfo(stylePainter, ptIcon, nInfoWidth, pszDefaultIcon);
}

void CSettingBluetoothItem::PaintDefault(QStylePainter & stylePainter, QPixmap & pmIcon,
        const QPoint & ptBegin, int nInfoWidth/* = 0*/)
{
    QRect rcIcon(ptBegin.x() + DEFAULT_INFO_POS_ICON_LEFT, ptBegin.y() + DEFALUT_INFO_POS_ICON_TOP,
                 DEFAULT_INFO_POS_ICON_WIDTH, DEFAULT_INFO_POS_ICON_HEIGHT);

    stylePainter.drawPixmap(rcIcon, pmIcon);

    QPoint ptInfo(rcIcon.right(), ptBegin.y());

    if (0 == nInfoWidth)
    {
        QRect rcItem = rect();

        nInfoWidth = rcItem.right() - ptInfo.x();
    }
    else
    {
        nInfoWidth = nInfoWidth - ptInfo.x();
    }

    PaintInfo(stylePainter, ptInfo, nInfoWidth);
}

void CSettingBluetoothItem::PaintIconInfo(QStylePainter & stylePainter, const QPoint & ptBegin,
        int nInfoWidth/* = 0*/, const char * pszDefaultIcon/* = NULL*/)
{
    QPoint ptIcon(ptBegin.x(), ptBegin.y() + DEFALUT_INFO_POS_ICON_TOP);

    PaintIcon(stylePainter, ptIcon, pszDefaultIcon);
    PaintDetailButton(stylePainter, this->rect().right());

    QPoint ptInfo(ptBegin.x() + DEFAULT_INFO_POS_ICON_WIDTH, ptBegin.y());

    if (0 == nInfoWidth)
    {
        QRect rcItem = rect();

        nInfoWidth = rcItem.right() - ptBegin.x();
    }
    else
    {
        nInfoWidth = nInfoWidth - ptBegin.x();
    }

    PaintInfo(stylePainter, ptInfo, nInfoWidth);
}

void CSettingBluetoothItem::PaintDetailButton(QStylePainter & stylePainter, int nRight)
{
    if (m_bluetoothData.bIsPairOrNot)
    {
        QRect rcIcon(nRight - DEFAULT_CONTACT_POS_ICON_DETAIL_X_OFFSET, DEFAULT_CONTACT_POS_ICON_DETAIL_TOP,
                     DEFAULT_CONTACT_POS_ICON_DETAIL_WIDTH, DEFAULT_CONTACT_POS_ICON_DETAIL_HEIGHT);

        // Move Btn点击区域太小，再向左扩大25
        m_btnDetail.SetRect(rcIcon.x() - 25, 0, this->width() - rcIcon.x() + 25, this->height());
#ifdef BLUETOOTH_USED_OLD_VERSION
        if (IsDown())
        {
            m_btnDetail.PaintButton(stylePainter, PIC_BLUE_OLD_MORE_WHITE, PIC_BLUE_OLD_MORE_PRESS, NULL,
                                    rcIcon);
        }
        else
        {
            m_btnDetail.PaintButton(stylePainter, PIC_BLUE_OLD_MORE, PIC_BLUE_OLD_MORE_PRESS, NULL, rcIcon);
        }
#endif
#ifdef BLUETOOTH_USED_NEW_VERSION
        if (IsDown())
        {
            m_btnDetail.PaintButton(stylePainter, PIC_BLUE_MORE_WHITE, PIC_BLUE_MORE_PRESS, NULL, rcIcon);
        }
        else
        {
            m_btnDetail.PaintButton(stylePainter, PIC_BLUE_MORE, PIC_BLUE_MORE_PRESS, NULL, rcIcon);
        }
#endif
    }
}

void CSettingBluetoothItem::PaintIcon(QStylePainter & stylePainter, const QPoint & ptIcon,
                                      const char * pszDefaultIcon/* = NULL*/)
{
    QRect rcIcon(ptIcon.x(), ptIcon.y(), DEFAULT_INFO_POS_ICON_WIDTH, DEFAULT_INFO_POS_ICON_HEIGHT);

    PaintIcon(stylePainter, rcIcon, pszDefaultIcon);
}

void CSettingBluetoothItem::PaintIcon(QStylePainter & stylePainter, const QRect & rcIcon,
                                      const char * pszDefaultIcon/* = NULL*/)
{
#ifdef IF_ENABLE_TESTABILITY
    m_rcIcon = rcIcon;
#endif
#ifdef SUPPORT_PHONE_DEVICE
    int iIdx = 0;
    if (m_bluetoothData.bIsPairOrNot && m_bluetoothData.bAutoConnect)
    {
        iIdx = 2;
    }
    iIdx += IsDown();
    QPixmap pmIcon = m_bluetoothData.bMobileDevice ? THEME_GET_BMP(g_pMobileBluetoothIcon[iIdx])
                     : THEME_GET_BMP(g_pBluetoothIcon[iIdx]);
    if (!pmIcon.isNull())
    {
        stylePainter.drawPixmap(rcIcon, pmIcon);
        return ;
    }
#else
    if (m_bluetoothData.bIsPairOrNot && m_bluetoothData.bAutoConnect)
    {
        // 按下时效果
        if (IsDown())
        {
            QPixmap pmIcon = THEME_GET_BMP(PIC_BLUE_AUTO_ICON_WHITE);
            if (!pmIcon.isNull())
            {
                stylePainter.drawPixmap(rcIcon, pmIcon);
                return ;
            }
        }
        else
        {
            QPixmap pmIcon = THEME_GET_BMP(PIC_BLUE_AUTO_ICON);
            if (!pmIcon.isNull())
            {
                stylePainter.drawPixmap(rcIcon, pmIcon);
                return ;
            }
        }
    }
    else
    {
        // 按下时效果
        if (IsDown())
        {
            QPixmap pmIcon = THEME_GET_BMP(PIC_BLUE_ICON_WHITE);
            if (!pmIcon.isNull())
            {
                stylePainter.drawPixmap(rcIcon, pmIcon);
                return ;
            }
        }
        else
        {
            QPixmap pmIcon = THEME_GET_BMP(PIC_BLUE_ICON);
            if (!pmIcon.isNull())
            {
                stylePainter.drawPixmap(rcIcon, pmIcon);
                return ;
            }
        }
    }
#endif

    if (NULL != pszDefaultIcon)
    {
        QPixmap pmDefault = THEME_GET_BMP(pszDefaultIcon);

        if (!pmDefault.isNull())
        {
            stylePainter.drawPixmap(rcIcon, pmDefault);
        }

        return;
    }
}

void CSettingBluetoothItem::PaintInfo(QStylePainter & stylePainter, const QPoint & ptText,
                                      int nRemainWidth)
{
    nRemainWidth -= DEFALUT_INFO_POS_TEXT_LEFT_MARGIN;

    int nLeft = ptText.x() + DEFALUT_INFO_POS_TEXT_LEFT_MARGIN;
    int nTitleTop = ptText.y() + DEFAULT_INFO_POS_TITLE_TOP;
    QRect rcTitle(nLeft, nTitleTop, nRemainWidth, DEFAULT_INFO_POS_TITLE_HEIGHT);

    int nInfoTop = ptText.y() + DEFAULT_INFO_POS_INFO_BOTTOM - DEFAULT_INFO_POS_INFO_HEIGHT;
    QRect rcInfo(nLeft, nInfoTop, nRemainWidth, DEFAULT_INFO_POS_INFO_HEIGHT);

    QRect rcNote(265, 10, 200, 30);

    PaintInfo(stylePainter, rcTitle, rcInfo, rcNote);
}

void CSettingBluetoothItem::PaintInfo(QStylePainter & stylePainter, const QRect & rcTitle,
                                      const QRect & rcInfo, const QRect & rcNote,
                                      int nTitleAlign/* = Qt::AlignTop*/, int nInfoAlgin/* = Qt::AlignBottom*/,
                                      int nNoteAlgin/* = Qt::AlignLeft*/)
{
#ifdef IF_ENABLE_TESTABILITY
    m_rcLblDis = rcTitle;
    m_rcLblMac = rcInfo;
    m_rcLblNote = rcNote;
#endif
    bool bEnable = isEnabled();
    const QPalette & txtPalette = palette();
    QFont fontText = stylePainter.font();
    // 按下时设置反白
    if (IsDown())
    {
        stylePainter.setPen(DEFAULT_SIMPLE_INFO_SELECT_TEXT_COLOR);
    }
    else
    {
        stylePainter.setPen(DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR);
    }

    if (!m_strInfo.isEmpty())
    {
        fontText.setPointSize(16);
        fontText.setWeight(QFont::Bold);
        fontText.setBold(false);
        stylePainter.setFont(fontText);

        stylePainter.drawItemText(rcTitle, (Qt::AlignLeft | nTitleAlign), txtPalette, bEnable, m_strInfo);
    }

    if (!m_bluetoothData.strMac.empty())
    {
        fontText.setPointSize(13);
        fontText.setWeight(QFont::Light);
        fontText.setBold(false);
        stylePainter.setFont(fontText);

        stylePainter.drawItemText(rcInfo, (Qt::AlignLeft | nInfoAlgin), txtPalette, bEnable,
                                  m_bluetoothData.strMac.c_str());
    }

    if (!m_bluetoothData.strNote.empty())
    {
        fontText.setPointSize(16);
        fontText.setWeight(QFont::Bold);
        fontText.setBold(false);
        stylePainter.setFont(fontText);

        stylePainter.drawItemText(rcNote, (Qt::AlignLeft | nNoteAlgin), txtPalette, bEnable,
                                  m_bluetoothData.strNote.c_str());

#ifdef BLUETOOTH_USED_OLD_VERSION
        // set note to disappear or not after 2 seconds
        if (!m_bluetoothData.bExistNoteOrNot)
        {
            QTimer::singleShot(2000, this, SLOT(RemoveNote()));
        }
#endif
#ifdef BLUETOOTH_USED_NEW_VERSION
        if (!m_bluetoothData.bExistNoteOrNot)
        {
            StartTimeClock();
        }
#endif
    }
}

void CSettingBluetoothItem::mousePressEvent(QMouseEvent * pEvent)
{
    if (!m_bluetoothData.bIsPairOrNot)
    {
        if (!ProcessMousePress(pEvent))
        {
            return;
        }

        pEvent->accept();
        return ;
    }

    if (!m_btnDetail.ProcessMousePress(pEvent))
    {
        if (!ProcessMousePress(pEvent))
        {
            return;
        }
    }

    pEvent->accept();
}

void CSettingBluetoothItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    if (!m_bluetoothData.bIsPairOrNot)
    {
        ProcessMouseRelease(pEvent, true);
        return ;
    }

    bool bDetailDown = false;
    bool bDetailClick = false;

    bDetailDown = m_btnDetail.IsDown();
    bDetailClick = m_btnDetail.ProcessMouseRelease(pEvent);

    if (!bDetailClick && !ProcessMouseRelease(pEvent, true))
    {
        if (bDetailDown)
        {
            m_btnDetail.ResetDown();
        }
        return;
    }

    if (bDetailClick)
    {
        DoAction(LIST_ITEM_BTN_CLICK);
    }
}

void CSettingBluetoothItem::SetItemData(QWidgetPair qPair)
{
#ifdef BLUETOOTH_USED_NEW_VERSION
    StopTimeClock();
#endif
    SetClickAction(qPair.third);
    m_bluetoothData = qPair.appendVariantData.value<BluetoothAddData>();

    SETTINGUI_INFO("[CSettingBluetoothItem::SetItemData] click[%s], note[%s]"
                   , qPair.third.toUtf8().data(), m_bluetoothData.strNote.c_str());
    update();
}

void CSettingBluetoothItem::StartTimeClock()
{
    if (NULL != m_pTimer && !m_pTimer->isActive())
    {
        m_pTimer->start(TIME_CLOCK_OUT);
    }
}

void CSettingBluetoothItem::StopTimeClock()
{
    if (NULL != m_pTimer && m_pTimer->isActive())
    {
        m_pTimer->stop();
    }
}

yl::string CSettingBluetoothItem::GetDeviceMac()
{
    return m_bluetoothData.strMac;
}

#ifdef SUPPORT_PHONE_DEVICE
bool CSettingBluetoothItem::IsMobileDevice() const
{
    return m_bluetoothData.bMobileDevice;
}

bool CSettingBluetoothItem::IsAutoSyncDevice() const
{
    return m_bluetoothData.bAutoSync;
}
#endif

#ifdef IF_ENABLE_TESTABILITY
QString CSettingBluetoothItem::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    //icon
    ptAbsolute = mapToGlobal(m_rcIcon.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcIcon.width();
    testInfo.height = m_rcIcon.height();
    testInfo.id = LABEL_ICON;
    testInfo.text = "";
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    // btn
    AddBtnNode(nodeRoot, m_btnDetail, BTN_DETAIL);

    // title
    ptAbsolute = mapToGlobal(m_rcLblDis.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcLblDis.width();
    testInfo.height = m_rcLblDis.height();
    testInfo.id = LABEL_TITLE;
    testInfo.text = fromQString(m_bluetoothData.strInfo);
    testInfo.type = VIRTUAL_LABEL;
    AddNode(nodeRoot, testInfo);

    // info
    ptAbsolute = mapToGlobal(m_rcLblMac.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcLblMac.width();
    testInfo.height = m_rcLblMac.height();
    testInfo.id = LABEL_INFO;
    testInfo.text = m_bluetoothData.strMac;
    testInfo.type = VIRTUAL_LABEL;
    AddNode(nodeRoot, testInfo);

    // info
    ptAbsolute = mapToGlobal(m_rcLblNote.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcLblNote.width();
    testInfo.height = m_rcLblNote.height();
    testInfo.id = LABEL_NOTE;
    testInfo.text = m_bluetoothData.strNote;
    testInfo.type = VIRTUAL_LABEL;
    AddNode(nodeRoot, testInfo);


    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif
