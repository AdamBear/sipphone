#ifndef __UC_FAVORITES_H__
#define __UC_FAVORITES_H__

#include "dsskey/include/dsskeystructure.h"
#include "dsskey/include/idsskeyimplaction.h"

class CDssKeyUCFavorite : public IDsskeyImplAction
{
public:
    CDssKeyUCFavorite();
    virtual ~CDssKeyUCFavorite();

public:
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

#endif
