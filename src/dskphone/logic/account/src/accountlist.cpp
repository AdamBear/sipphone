#include "accountlist.h"



CAccountList::CAccountList()
{
}


CAccountList::~CAccountList()
{
    for (iterator it = begin(); it != end(); ++it)
    {
        if (NULL != *it)
        {
            delete *it;
        }
        *it = NULL;
    }
}

CAccountList::iterator CAccountList::erase(CAccountList::iterator _Where)
{
    if (_Where == end())
    {
        return _Where;
    }
    if (NULL != *_Where)
    {
        delete *_Where;
    }
    *_Where = NULL;
    return YLList<CBaseAccount *>::erase(_Where);
}

void CAccountList::clear()
{
    for (iterator it = begin(); it != end();)
    {
        it = erase(it);
    }
}
