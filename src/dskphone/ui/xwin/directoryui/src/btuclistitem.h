#ifndef __BTUC_LIST_ITEM_H__
#define __BTUC_LIST_ITEM_H__

#if FEATURE_BROADTOUCH_UC

#include <ylstring.h>
#include "xwindow/xlistviewitem.h"

enum
{
    LIST_ITEM_TYPE_BTUC_CONTACT     = 0x09000000,
};

class xImageView;
class xTextView;

class CBTUCListItem : public xListViewItem
{
public:
    explicit CBTUCListItem(IListViewItemAction * pAction = NULL);
    virtual ~CBTUCListItem();

    static int GetBTUCItemType();
    static CBTUCListItem* GetBTUCListItem(xListViewItem * pItem);

    static xListViewItem * CreateBTUCListItem();

public:
    virtual void onLoadChildElement(xml_node & nodeChild);
    virtual bool onFocusEvent(bool bFocusIn);

public:
    void SetId(int nId);
    void SetPresence(const yl::string& strPresence);
    void SetName(const yl::string& strName);
    void SetSignature(const yl::string& strSignature);
    void SetPhoto(const yl::string& strPhoto);
    int GetId() const;

protected:
    int m_nId;
    xImageView* m_pImagePhoto;
    xImageView* m_pImagePresence;
    xTextView* m_pTextName;
    xTextView* m_pTextSignature;
};
typedef CBTUCListItem * CBTUCListItemPtr;

#endif //FEATURE_BROADTOUCH_UC
#endif //__BTUC_LIST_ITEM_H__
