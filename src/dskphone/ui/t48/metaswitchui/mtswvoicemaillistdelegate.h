#ifndef MTSWVOICEMAILLISTDELEGATE_H
#define MTSWVOICEMAILLISTDELEGATE_H

#include "config.h"
#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "settingui/src/subpagelistdelegate.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"

class MTSWVoicemailListDelegate : public SubpageListDeleagate
{
    Q_OBJECT

public:
    MTSWVoicemailListDelegate();
    ~MTSWVoicemailListDelegate();

    void PreCreateItem();
    void HideItem();

    void SetItemPat(QWidget * pPat)
    {
        m_pItemPat = pPat;
    }

    virtual void BindFramelist(CFrameList * pList);
    virtual void UnBindFramelist();

    // set widget pair
    void SetItemsData(QWPVector vecWgtpair, int iTotal);

    void ReloadData(int nDataIndex, int nDataCount);

    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

    void CloseAllTimer(SettingItemList & m_vecNorItem);

signals:
    void ItemMoreAction(CListItem * pItem);

    void ItemStopAction(CListItem * pItem);

    void ItemFinishAction(CListItem * pItem);

private:
    CFrameList     *    m_pVoicemailFramelist;
    QWPVector           m_vecWgtPair;
    QWidget      *      m_pItemPat;
    int                 m_iTotal;
};

#endif
#endif
