#ifdef IF_FEATURE_DSSKEY

#include "talkdsskey_inc.h"

CDsskeyEdkAction::CDsskeyEdkAction()
    : CDssTalkBaseAciton(DT_CUSTOM_KEY)
{

}

CDsskeyEdkAction::~CDsskeyEdkAction()
{

}

bool CDsskeyEdkAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                  void* pExtraData)
{

#if IF_FEATURE_EDK
    EDK_DoAction(logicData.strValue);
#endif
    return true;
}
#endif
