#ifndef __HOTDESKING_DSSKEYHOTDESKINGACTION_H__
#define __HOTDESKING_DSSKEYHOTDESKINGACTION_H__

#include "hotdesking_inc.h"


class CDsskeyHotDeskingAction
    : public IDsskeyImplAction
{
public:
    CDsskeyHotDeskingAction();

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

#endif // __HOTDESKING_DSSKEYHOTDESKINGACTION_H__
