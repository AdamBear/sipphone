#ifndef _E911_LOCATION_DELEGATE_H
#define _E911_LOCATION_DELEGATE_H

#include "config.h"
#if IF_FEATURE_GENBAND_E911
#include "settingui/src/subpagelistdelegate.h"

class CE911LocationDelegate : public SubpageListDeleagate
{
    Q_OBJECT

public:
    CE911LocationDelegate();

#ifndef _T48
    CE911LocationDelegate(CFrameList* pList);
#endif

    ~CE911LocationDelegate();

    void SetItemPat(QWidget* pPat)
    {
        m_pItemPat = pPat;
    }
    void HideItem();

    void PreCreateItem();

    virtual void BindFramelist(CFrameList*  pList);
    virtual void UnBindFramelist();

    virtual void OnLoadData(CFrameListBase* pFrameList, int nDataIndex, int nDataCount);

    virtual int GetMaxPagePageItemCount();

    int GetFocusItemID();

    bool FocusItemHasParentItem();
    bool FouceItemHasChildItem();

    void SetPageID(int nPageID);
    int GetPageID();

    yl::string GetCurrentPageTitle();

    void SetScrollViewType(int nType);

public:

private:
    CFrameList*     m_pE911LocationFramelist;
    QWidget*            m_pItemPat;

    int m_nPageID;
};
#endif
#endif //_E911_LOCATION_DELEGATE_H
