#ifndef BLUETOOTHDELEGATE_H
#define BLUETOOTHDELEGATE_H

#include "subpagelistdelegate.h"

class BluetoothDelegate : public SubpageListDeleagate
{
    Q_OBJECT

public:
    BluetoothDelegate();
    ~BluetoothDelegate();

    /******************************IListItemAction************************/
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

    void SetItemDataByDataIndex(QWidgetPair qPair, int iDateIdx,
                                int eItemType = SETTING_ITEM_BLUETOOTH);

private:
signals:
    void ItemDetailAction(CListItem * pItem);
};

#endif
