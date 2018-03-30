#include "usbphotoitem.h"

#include "baseui/t4xpicpath.h"
#include "baseui/t4xpicpath.h"
#include "uikernel/ilistitemdelegate.h"
#include "uikernel/languagehelper.h"
#include "uicommon/imagemanager/modimagemanager.h"
#include "settingitembase.h"
#include "actiondefinecommon.h"

namespace
{
//文件名位置
#define USB_PHOTO_ITEM_NAME_LABEL_LEFT              87
#define USB_PHOTO_ITEM_NAME_LABEL_TOP               7
#define USB_PHOTO_ITEM_NAME_LABEL_WIDTH             340
#define USB_PHOTO_ITEM_NAME_LABEL_HEIGHT            20

//时间和大小信息位置
#define USB_PHOTO_ITEM_DETAIL_LABEL_LEFT            87
#define USB_PHOTO_ITEM_DETAIL_LABEL_TOP             29
#define USB_PHOTO_ITEM_DETAIL_LABEL_WIDTH       100
#define USB_PHOTO_ITEM_DETAIL_LABEL_HEIGHT      20

#define USB_PHOTO_TASKACTION_MAX_COUNT          2
}

CUSBPhotoItem::CUSBPhotoItem(QWidget* pParent, QWidgetPair& pair)
    : CUSBItemBase(pParent, pair)
    , m_nIndex(0)
    , m_bIsImageLoaded(false)
    , m_bCanLoad(true)
{
    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_PHOTO;

    Relayout();

    m_picThumbnail.setParent(this);
}


CUSBPhotoItem::~CUSBPhotoItem(void)
{
}

int CUSBPhotoItem::GetUSBListItemType()
{
    return (LIST_ITEM_TYPE_SETTING | LIST_ITEM_SUB_TYPE_SETTING_PHOTO);
}

void CUSBPhotoItem::paintEvent(QPaintEvent* pEvent)
{
    ApplyNomalStyle();

    QStylePainter stylePainter(this);
    PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());

    m_btnLeft.PaintButton(stylePainter, PIC_ICON_USB_PHOTO_LIST_ICON, PIC_ICON_USB_PHOTO_LIST_ICON, "",
                          USB_ITEM_LEFT_BTN_PIC_RECT);
    m_btnRight.PaintButton(stylePainter, PIC_USB_BROWSE_ITEM_DELETE, PIC_USB_BROWSE_ITEM_DELETE_PRESS, "", USB_ITEM_RIGHT_DELETE_BTN_PIC_RECT);

}

void CUSBPhotoItem::Relayout()
{
    if (NULL == m_pLabDisplay)
    {
        return;
    }

    m_pLabDisplay->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_pLabDisplay->setGeometry(USB_PHOTO_ITEM_NAME_LABEL_LEFT
                               , USB_PHOTO_ITEM_NAME_LABEL_TOP
                               , USB_PHOTO_ITEM_NAME_LABEL_WIDTH
                               , USB_PHOTO_ITEM_NAME_LABEL_HEIGHT);

    m_labelBottom.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_labelBottom.setGeometry(USB_PHOTO_ITEM_DETAIL_LABEL_LEFT
                              , USB_PHOTO_ITEM_DETAIL_LABEL_TOP
                              , USB_PHOTO_ITEM_DETAIL_LABEL_WIDTH
                              , USB_PHOTO_ITEM_DETAIL_LABEL_HEIGHT);
    m_btnLeft.SetRect(USB_ITEM_LEFT_BTN_LEFT, USB_ITEM_LEFT_BTN_TOP
                      , USB_ITEM_LEFT_BTN_WIDTH, USB_ITEM_LEFT_BTN_HEIGHT);
    m_btnRight.SetRect(USB_ITEM_RIGHT_BTN_LEFT, USB_ITEM_RIGHT_BTN_TOP
                       , USB_ITEM_RIGHT_BTN_WIDTH, USB_ITEM_RIGHT_BTN_HEIGHT);
    update();
}

void CUSBPhotoItem::mousePressEvent(QMouseEvent* pEvent)
{
    if (NULL == pEvent)
    {
        return;
    }

    //右边按钮判断
    if (m_btnRight.ProcessMousePress(pEvent))
    {
        return;
    }

    //item判断
    ProcessMousePress(pEvent);
}

void CUSBPhotoItem::mouseReleaseEvent(QMouseEvent* pEvent)
{
    //右边按钮判断
    if (m_btnRight.ProcessMouseRelease(pEvent))
    {
        UILogicDoAction(USB_CLICK_ACTION_DELETE);
        return;
    }

    //item判断
    ProcessMouseRelease(pEvent);
}


