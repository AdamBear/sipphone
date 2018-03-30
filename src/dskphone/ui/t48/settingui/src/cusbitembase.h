#ifndef _C_USB_ITEM_BASE_H
#define _C_USB_ITEM_BASE_H

#include <QtGui>
#include "settingui/src/settingitembase.h"
#include "usbuicommon.h"

namespace
{
//左右两个按钮的实际点击区域
#define USB_ITEM_LEFT_BTN_LEFT              0
#define USB_ITEM_LEFT_BTN_TOP               0
#define USB_ITEM_LEFT_BTN_WIDTH             65
#define USB_ITEM_LEFT_BTN_HEIGHT            50

#define USB_ITEM_RIGHT_BTN_LEFT             432
#define USB_ITEM_RIGHT_BTN_TOP              0
#define USB_ITEM_RIGHT_BTN_WIDTH            65
#define USB_ITEM_RIGHT_BTN_HEIGHT           50


//左右两个按钮的图片的位置（小于实际点击区域）
#define USB_ITEM_LEFT_BTN_PIC_LEFT          32
#define USB_ITEM_LEFT_BTN_PIC_TOP           13

#define USB_ITEM_RIGHT_BTN_PIC_LEFT         432
#define USB_ITEM_RIGHT_BTN_PIC_TOP          13

#define USB_ITEM_BTN_PIC_WIDTH              30
#define USB_ITEM_BTN_PIC_HEIGHT             30

#define USB_ITEM_RIGHT_DELETE_BTN_PIC_LEFT  432
#define USB_ITEM_RIGHT_DELETE_BTN_PIC_TOP   15
#define USB_ITEM_RIGHT_DELETE_BTN_PIC_WIDTH     25
#define USB_ITEM_RIGHT_DELETE_BTN_PIC_HEIGHT    25

#define USB_ITEM_LEFT_BTN_PIC_RECT              QRect(USB_ITEM_LEFT_BTN_PIC_LEFT, USB_ITEM_LEFT_BTN_PIC_TOP, USB_ITEM_BTN_PIC_WIDTH, USB_ITEM_BTN_PIC_HEIGHT)
#define USB_ITEM_RIGHT_BTN_PIC_RECT             QRect(USB_ITEM_RIGHT_BTN_PIC_LEFT, USB_ITEM_RIGHT_BTN_PIC_TOP, USB_ITEM_BTN_PIC_WIDTH, USB_ITEM_BTN_PIC_HEIGHT)
#define USB_ITEM_RIGHT_DELETE_BTN_PIC_RECT      QRect(USB_ITEM_RIGHT_DELETE_BTN_PIC_LEFT, USB_ITEM_RIGHT_DELETE_BTN_PIC_TOP, USB_ITEM_RIGHT_DELETE_BTN_PIC_WIDTH, USB_ITEM_RIGHT_DELETE_BTN_PIC_HEIGHT)
}

class CUSBItemBase : public CSettingItem
{
    Q_OBJECT

public:
    CUSBItemBase(QWidget * pParent, QWidgetPair & pair);
    ~CUSBItemBase();

    virtual void ResetItemInfo(const QWidgetPair & itemData);

    void SetBottomLabel(const QString & strHint);

    virtual void LoadPlayData(long nTotalSeconds, long nCurrentSeconds);

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent) = 0;
    virtual void mouseReleaseEvent(QMouseEvent * pEvent) = 0;

    virtual void Relayout() { }

#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
    virtual void ApplyNomalStyle();
#endif

    virtual bool UILogicDoAction(int nAction);

protected:
    //按钮图标所在位置
    CButtonWrapper  m_btnLeft;
    CButtonWrapper  m_btnRight;

    QLabel          m_labelBottom;
};

#endif // CUSBITEMBASE_H
