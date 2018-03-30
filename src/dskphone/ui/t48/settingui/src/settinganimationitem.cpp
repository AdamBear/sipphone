#include "settinganimationitem.h"
#include "talkui/src/qpictureanimation.h"
#include "baseui/framelistaction.h"
#include <QStylePainter>

Q_DECLARE_METATYPE(SSettingAnimationItemData);

CSettingAnimationItem::CSettingAnimationItem(QWidget * parent, int iDataIndex/* = -1*/,
        bool bAddInex/* = false*/)
    : CSettingItem(parent)
    , m_pPicAnimation(NULL)
{
    m_bAddIndex = bAddInex;
    m_pWgt = NULL;
    SetDataIndex(iDataIndex);
    GenerateLabelText();

    InitStyle();
}

CSettingAnimationItem::CSettingAnimationItem(QWidget * pParent, QWidgetPair & pair)
    : CSettingItem(pParent, pair.first, pair.second, pair.third, pair.iIndex, false)
    , m_pPicAnimation(NULL)
{
    InitStyle();

    if (pair.second != NULL && pair.second->inherits("qPicAnimationBase"))
    {
        m_pPicAnimation = static_cast<qPicAnimationBase *>(pair.second);
        SetItemData(pair);
    }
}

CSettingAnimationItem::~CSettingAnimationItem()
{
}

int CSettingAnimationItem::GetSettingAnimationItemType()
{
    return (LIST_ITEM_TYPE_SETTING | LIST_ITEM_SUB_TYPE_ANIMATION);
}

void CSettingAnimationItem::StopAnimation()
{
    if (NULL == m_pPicAnimation)
    {
        return;
    }

    m_pPicAnimation->Stop();

    if (m_sAnimationItemData.m_bHideAnimationWhenStop)
    {
        m_pPicAnimation->hide();
        Relayout();
    }
}

void CSettingAnimationItem::StartAnimation()
{
    if (NULL == m_pPicAnimation)
    {
        return;
    }

    if (m_sAnimationItemData.m_bHideAnimationWhenStop)
    {
        m_pPicAnimation->show();
        Relayout();
    }

    m_pPicAnimation->Play();
}

void CSettingAnimationItem::Relayout()
{
    if (NULL == m_pPicAnimation || NULL == m_pLabDisplay)
    {
        return;
    }

    if (m_sAnimationItemData.m_bAnimationSwitch || !m_sAnimationItemData.m_bHideAnimationWhenStop)
    {
        m_pPicAnimation->setGeometry(m_sAnimationItemData.m_rcAnimationWidget);
        m_pLabDisplay->setGeometry(m_sAnimationItemData.m_rcText);
    }
    else
    {
        QRect rcTextLabel = m_sAnimationItemData.m_rcAnimationWidget;
        rcTextLabel.setWidth(m_sAnimationItemData.m_rcText.right() - m_sAnimationItemData.m_rcText.left());
        rcTextLabel.setHeight(m_sAnimationItemData.m_rcText.bottom() - m_sAnimationItemData.m_rcText.top());

        m_pLabDisplay->setGeometry(rcTextLabel);
        m_pPicAnimation->setGeometry(0, 0, 0, 0);
    }
}

void CSettingAnimationItem::SetItemData(QWidgetPair qPair)
{
    m_sAnimationItemData = qPair.appendVariantData.value<SSettingAnimationItemData>();

    Relayout();

    if (m_pLabDisplay != NULL)
    {
        m_strLabel =  m_sAnimationItemData.m_strLabel;
    }

    if (m_sAnimationItemData.m_bAnimationSwitch)
    {
        StartAnimation();
    }
    else
    {
        StopAnimation();
    }
}

void CSettingAnimationItem::InitStyle()
{
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);
#endif

    m_nType |= LIST_ITEM_SUB_TYPE_ANIMATION;
}
