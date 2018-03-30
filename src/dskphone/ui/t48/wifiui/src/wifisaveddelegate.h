#ifndef __WIFI_SAVED_DELEGATE_H
#define __WIFI_SAVED_DELEGATE_H

#include "settingui/src/subpagelistdelegate.h"

class WifiSavedDelegate : public SubpageListDeleagate
{
    Q_OBJECT

public:
    WifiSavedDelegate();
    ~WifiSavedDelegate();
#ifdef _T49
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);
#endif
    /******************************IListItemAction************************/
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

private:
signals:
    void ItemDetailAction(CListItem * pItem);
#ifdef _T49
    void ItemDragDropStart(CListItem * pItem);
    void ItemDragDropEnd(CListItem * pItem);
#endif
};

#endif
