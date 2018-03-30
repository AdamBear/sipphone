#ifndef __SETTING_ANIMATION_ITEM_H__
#define __SETTING_ANIMATION_ITEM_H__

#include "settingitembase.h"

struct SSettingAnimationItemData
{
    QRect m_rcAnimationWidget;
    QRect m_rcText;
    bool m_bHideAnimationWhenStop;
    bool m_bAnimationSwitch;
    QString m_strLabel;

    SSettingAnimationItemData()
    {
        m_bHideAnimationWhenStop = false;
        m_bAnimationSwitch = false;
    }
};

class qPicAnimationBase;

class CSettingAnimationItem : public CSettingItem
{
    Q_OBJECT

public:
    CSettingAnimationItem(QWidget * parent, int iDataIndex = -1, bool bAddInex = false);
    CSettingAnimationItem(QWidget * pParent, QWidgetPair & pair);
    ~CSettingAnimationItem();

    static int GetSettingAnimationItemType();

    void StartAnimation();
    void StopAnimation();
    void SetItemData(QWidgetPair qPair);

protected:
    virtual void Relayout();

    void InitStyle();

private:
    qPicAnimationBase     *     m_pPicAnimation;
    SSettingAnimationItemData   m_sAnimationItemData;
};

#endif // #ifndef __SETTING_ANIMATION_ITEM_H__
