#include "transparencymsghelper.h"

void CTransChangeMsgHelper::OnTransparencyChange()
{
    emit transparencyChange();
}

CTransChangeMsgHelper & GetTransChangeMsgHelperInstance()
{
    static CTransChangeMsgHelper s_TransChangeMsgHelper;
    return s_TransChangeMsgHelper;
}
