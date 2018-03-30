#ifndef __ACCOUNT_LIST_H__
#define __ACCOUNT_LIST_H__

#include "account_inc.h"
#include "baseaccount.h"

class CAccountList : public YLList<CBaseAccount *>
{
public:
    CAccountList();
    virtual ~CAccountList();

    iterator erase(iterator _Where);

    void clear();
};


#endif // !__ACCOUNT_LIST_H__
