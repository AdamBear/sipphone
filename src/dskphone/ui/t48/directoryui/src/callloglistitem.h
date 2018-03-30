#ifndef __CALL_LOG_LIST_ITEM_H__
#define __CALL_LOG_LIST_ITEM_H__

#include "baseui/contactbaseitem.h"


enum
{
    LIST_ITEM_SUB_TYPE_CALL_LOG         = 0x00000100,
};

class CCallLogListItem : public CContactBaseItem
{
    Q_OBJECT

public:
    CCallLogListItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CCallLogListItem(void);

    static int GetCallLogListItemType();

public:
    void SetCallType(int nType);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

protected:
    int m_nCallType;
};
typedef CCallLogListItem * CCallLogListItemPtr;

#endif
