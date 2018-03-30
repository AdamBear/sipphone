#if IF_FEATURE_METASWITCH_ACD
#include "mtswacdactioncmd.h"
#include "callcenter_inc.h"

CMTSWACDActionCmd::CMTSWACDActionCmd(void)
{
}

CMTSWACDActionCmd::~CMTSWACDActionCmd(void)
{
}

void CMTSWACDActionCmd::OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType, int nId)
{
    if (g_ACDModel != NULL
            && g_ACDModel->Get_ACDType() == ACD_METASWITCH)
    {
        ((CACDModelMTSW *)g_ACDModel)->OnRequestError(eErrCode, nType);
    }
}

bool CMTSWACDActionCmd::OnLoadSucProcess(CMTSWAction * pAction)
{
    if (g_ACDModel != NULL
            && g_ACDModel->Get_ACDType() == ACD_METASWITCH
            && pAction != NULL)
    {
        return ((CACDModelMTSW *)g_ACDModel)->OnMTSWActionResponse(pAction->GetJsonObject(),
                pAction->GetMTSWActionType());
    }

    return false;
}
#endif
