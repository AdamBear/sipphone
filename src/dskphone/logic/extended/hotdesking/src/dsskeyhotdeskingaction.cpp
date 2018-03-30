#include "dsskeyhotdeskingaction.h"
#include "modhotdesking.h"

CDsskeyHotDeskingAction::CDsskeyHotDeskingAction()
    : IDsskeyImplAction(DT_HOTDESKING)
{

}

bool CDsskeyHotDeskingAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
        void * pExtraData)
{
    hotDesking_Menu_Enter();
    return true;
}
