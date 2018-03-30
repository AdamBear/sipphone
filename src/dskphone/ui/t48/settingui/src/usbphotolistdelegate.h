#ifndef USBPHOTOLLISTDELEGATE_H
#define USBPHOTOLLISTDELEGATE_H

#include "settingui/src/subpagelistdelegate.h"
#include "record/include/modrecord.h"
#include "settingui/src/usbphotoitem.h"


class USBPhotoListDelegate : public SubpageListDeleagate
{
    Q_OBJECT

public:
    USBPhotoListDelegate();
    ~USBPhotoListDelegate();

    void SetItemPat(QWidget * pPat)
    {
        m_pItemPat = pPat;
    }
    void HideItem();

    void PreCreateItem();

    virtual void BindFramelist(CFrameList * pList);
    virtual void UnBindFramelist();

    void ReloadData(int nDataIndex, int nDataCount);
    void SetItemsData(QWPVector vecWgtpair, int iTotal);

    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

    //刷新列表，重新加载数据
    virtual void UpdateList();

signals:
    void ItemDeleteAction(CListItem * pItem);

private:
    CFrameList   *  m_pUSBPhotoFramelist;
    QWidget    *    m_pItemPat;
    QWPVector       m_vecWgtPair;
    int             m_iTotal;
};

#endif
