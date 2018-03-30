#ifndef USBRECORDLLISTDELEGATE_H
#define USBRECORDLLISTDELEGATE_H

#include "settingui/src/subpagelistdelegate.h"
#include "record/include/modrecord.h"
#include "settingui/src/usbaudioitem.h"

struct SItemPlayInfo
{
    int nIndex;
    yl::string strFile;
    USB_PLAY_STATE  PlayState;

    SItemPlayInfo()
    {
        Reset();
    }

    void Reset()
    {
        nIndex = -1;
        strFile = "";
        PlayState = USB_PLAY_STATE_STOP;
    }
};

class USBRecordListDelegate : public SubpageListDeleagate
{
    Q_OBJECT

public:
    USBRecordListDelegate();
    ~USBRecordListDelegate();

    void SetItemPat(QWidget * pPat)
    {
        m_pItemPat = pPat;
    }
    void HideItem();
    // 页面改变
    void PageChange(bool bHide);
    // 重新加载数据
    void ReloadList();

    void PreCreateItem();

    virtual void BindFramelist(CFrameList * pList);
    virtual void UnBindFramelist();

    void ReloadData(int nDataIndex, int nDataCount);
    void SetItemsData(QWPVector vecWgtpair, int iTotal);

    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

    virtual bool ProcessKeyEvent(QObject * pObject, QEvent * pEvent);

    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

signals:
    void ItemDeleteAction(CListItem * pItem);
    //将非焦点Item状态置为wait
    void SetOtherItemStateWait(CUSBAudioItem *);
    //将非焦点Item状态置为stop
    void SetOtherItemStateStop(CUSBAudioItem *);

private Q_SLOTS:
    void OnItemPlayStatusChange(CUSBAudioItem * pItem);
    void OnItemPlayFail(CUSBAudioItem * pItem);

private:
    CFrameList   *  m_pUSBRecordFramelist;
    QWidget    *    m_pItemPat;
    QWPVector       m_vecWgtPair;
    int             m_iTotal;
    SItemPlayInfo   m_sItemPlayInfo;
};

#endif
