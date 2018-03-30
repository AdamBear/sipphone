#ifndef ZEROLISTDELEGATE_H
#define ZEROLISTDELEGATE_H

#include "settingui/src/subpagelistdelegate.h"

class ZeroListDelegate : public SubpageListDeleagate
{
    Q_OBJECT

public:
    ZeroListDelegate();
    ~ZeroListDelegate();

    void PreCreateItem();
    void SetItemPat(QWidget * pPat)
    {
        m_pItemPat = pPat;
    }

    void HideItem();

    // bind and unbind a framelist
    virtual void BindFramelist(CFrameList * pList);
    virtual void UnBindFramelist();

    // set widget pair
    void SetItemsData(QWPVector vecWgtpair);

    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

    virtual bool ProcessMouse(QObject* pObj);
    virtual void SetKeyboardEnterType();
private:
    CFrameList     *    m_pZeroFramelist;
    QWPVector           m_vecWgtPair;
    QWidget      *      m_pItemPat;
};

#endif
