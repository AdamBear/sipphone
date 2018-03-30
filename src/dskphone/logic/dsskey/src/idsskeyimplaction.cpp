#include "../include/idsskeyimplaction.h"
#include "dsskeyactionmanager.h"
#include "dsskeyaction.h"

IDsskeyImplAction::IDsskeyImplAction(DssKey_Type type)
{
    CDSSKeyAction * pAction = _DSSKeyActionManger.GetDSSKeyAction(type);
    if (NULL != pAction)
    {
        pAction->SetImplAction(this);
        //监听自身的类型改变
        pAction->AddChangeListenType(type);
    }
    m_type = type;
}
IDsskeyImplAction::~IDsskeyImplAction()
{
    CDSSKeyAction * pAction = _DSSKeyActionManger.GetDSSKeyAction(m_type);
    if (pAction)
    {
        pAction->SetImplAction(NULL);
    }
    m_type = DT_NA;
}

IDsskeyImplAction* IDsskeyImplAction::GetImplAction(DssKey_Type eType)
{
    CDSSKeyAction * pAction = _DSSKeyActionManger.GetDSSKeyAction(eType);
    if (pAction)
    {
        return pAction->GetImplAction();
    }

    return NULL;
}
