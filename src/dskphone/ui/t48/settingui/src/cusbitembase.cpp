#include "cusbitembase.h"
#include "baseui/t4xpicpath.h"

CUSBItemBase::CUSBItemBase(QWidget * pParent, QWidgetPair & pair)
    : CSettingItem(pParent, pair, pair.bAddIndex)
    , m_btnLeft(this)
    , m_btnRight(this)
    , m_labelBottom(this)
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

    SetItemEnabled(false);
    m_labelBottom.setText(pair.strAuthorId.c_str());
    m_labelBottom.setAttribute(Qt::WA_TranslucentBackground, true);
}


CUSBItemBase::~CUSBItemBase()
{

}

void CUSBItemBase::ApplyNomalStyle()
{
    QPalette pal;

    QColor color = USB_COLOR_GRAY;
    pal = m_labelBottom.palette();
    pal.setBrush(QPalette::WindowText, QBrush(color));
    m_labelBottom.setPalette(pal);

    color = USB_COLOR_BLACK;
    if (NULL != m_pLabDisplay)
    {
        pal = m_pLabDisplay->palette();
        pal.setBrush(QPalette::WindowText, QBrush(color));
        m_pLabDisplay->setPalette(pal);
    }
}

void CUSBItemBase::ResetItemInfo(const QWidgetPair & itemData)
{
    if (NULL != m_pLabDisplay)
    {
        m_pLabDisplay->setText(itemData.first);
    }
    m_labelBottom.setText(itemData.strAuthorId.c_str());

    update();
}

void CUSBItemBase:: SetBottomLabel(const QString & strHint)
{
    m_labelBottom.setText(strHint);
    update();
}

bool CUSBItemBase::UILogicDoAction(int nAction)
{
    //传到代理 OnItemAction 处理
    DoAction(nAction);
    return true;
}

void CUSBItemBase::LoadPlayData(long nTotalSeconds, long nCurrentSeconds)
{

}
