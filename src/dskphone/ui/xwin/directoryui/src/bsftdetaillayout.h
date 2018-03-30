#ifndef __BSFT_DETAIL_LAYOUT_H__
#define __BSFT_DETAIL_LAYOUT_H__

#include "detaillayout.h"

enum
{
    DETAIL_LIST_ITEM_LAYOUT_LOCAL           = DETAIL_LIST_ITEM_LAYOUT_BASE + 2,
};

// BroadSoft联系人的界面顺序
enum
{
    BROADSOFT_CONTACT_DETAIL_ITEM_NAME      = 0,
    BROADSOFT_CONTACT_DETAIL_ITEM_NUMBER,
};

class CEditListItem;
struct ContactBroadSoftPersonal;

class CBSFTDetailLayout : public CDetailLayout
{
public:
    CBSFTDetailLayout();
    virtual ~CBSFTDetailLayout();

public:
    virtual void UnInit();

    virtual void ReturnAllEdit();

    void Init();

    void SetAction(int nAction);
    bool UpdateContact(int nContactId, ContactBroadSoftPersonal * pContact);

    int GetAction() const;
    int GetDetailId() const;

    CEditListItem * GetNameEditItem();
    CEditListItem * GetNumberEditItem();

    bool IsCopyLayout() const;
    bool IsAdditionLayout() const;

protected:
    void InitNameEditItem();
    void InitNumberEditItem();

protected:
    int m_nAction;
    int m_nDetailId;
};

#endif
