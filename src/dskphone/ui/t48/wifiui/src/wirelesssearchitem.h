#ifndef __WIRELESS_SEARCH_ITEM_H
#define __WIRELESS_SEARCH_ITEM_H

#include "settingui/src/settingitembase.h"
#include "wifiui/include/wirelesscommondata.h"

class NoteLabel;

class WirelessSearchStatisticsItem : public CSettingItem
{
    Q_OBJECT

public:
    WirelessSearchStatisticsItem(QWidget * parent, int iDataIndex = -1, bool bAddInex = false);
    WirelessSearchStatisticsItem(QWidget * pParent, QWidgetPair & pair);
    ~WirelessSearchStatisticsItem();

    static int GetWirelessSearchStatisticsItemType();

    void SetDisplayAndType(const QString & strDisplay, Item_Note_Type eType);

    void SetItemData(QWidgetPair qPair);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();

private:
    NoteLabel           *           m_pLblNote;
    WirelessSearchStatisticsData    m_ItemData;
};

#endif
