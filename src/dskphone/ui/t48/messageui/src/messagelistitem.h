#ifndef MESSAGELISTITEM_H
#define MESSAGELISTITEM_H

#include "settingui/src/settingitembase.h"
#include "ylstring.h"

class CMessageListItem : public CSettingItem
{
    Q_OBJECT
public:
    CMessageListItem(QWidget * parent, QString strIcon, QString strLabel
                     , QString strAction = "", bool bAddIcon = false
                     , int iDataIndex = -1, bool bAddInex = true, bool bWithStatus = false);
    CMessageListItem(QWidget * pParent, QWidgetPair & pair);
    ~CMessageListItem();

    static int GetMsgListItemType();

    void AddIcon(LPCSTR strIconPath);
protected:
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();

#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
    virtual void ApplyNomalStyle();
#endif

private:
    CButtonWrapper m_btnEnter;
    QPixmap     m_iconMap;
    bool        m_bAddIcon;
    bool        m_bWithStatus;//是否显示状态
};

#endif
