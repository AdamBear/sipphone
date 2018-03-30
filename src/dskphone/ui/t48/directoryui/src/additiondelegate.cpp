#include "additiondelegate.h"
#include "directorylistaction.h"
#include "btuccontactcontroler.h"
#include "commondirctrl.h"
#include "ldapcontactcontroler.h"
#include "baseui/framelist.h"
#include "localcontactuihelper.h"
#include "bsftcontactuihelper.h"
#include "bsftcallloguihelper.h"
#if IF_FEATURE_GBBOOK
#include "genbandcontactuihelper.h"
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactuihelper.h"
#endif

#include "configparser/modconfigparser.h"
#include <configiddefine.h>

CAdditionDelegate::CAdditionDelegate()
    : CDirectoryListDelegate(DIR_UI_STATE_ADDITION)
{
}

CAdditionDelegate::~CAdditionDelegate(void)
{
}

CAdditionDelegate * CAdditionDelegate::GetDirDelegate(CDirectoryListDelegatePtr & pDelegate)
{
    if (pDelegate.IsEmpty() || DIR_UI_STATE_ADDITION != pDelegate->GetState())
    {
        return NULL;
    }

    return (CAdditionDelegate *)pDelegate.Get();
}

void CAdditionDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CAdditionDelegate::OnLoadData(FrameList:%p DataIndex:%d DataCount:%d)",
               pFrameList, nDataIndex, nDataCount);

    CDirUIHelperPtr pUIHelper = GetUIHelper();

    if (NULL == pFrameList || pFrameList != m_pFrameList || pUIHelper.IsEmpty())
    {
        return;
    }

#if IF_FEATURE_GBBOOK
    CGenbandContactUIHelper * pGBUIHelper = CGenbandContactUIHelper::GetUIHelper(pUIHelper);

    if (NULL != pGBUIHelper)
    {
        pGBUIHelper->LoadGenbandAdditionData(DIR_ACTION_GENBAND_ADD_PAB, nDataIndex, 7);
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    CMTSWContactUIHelper * pMTSWUIHelper = CMTSWContactUIHelper::GetUIHelper(pUIHelper);

    if (NULL != pMTSWUIHelper)
    {
        pMTSWUIHelper->LoadMTSWAdditionData(DIR_ACTION_MTSW_ADD_CONTACTLIST, nDataIndex, 7);
    }
#endif
}
