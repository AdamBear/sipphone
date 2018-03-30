#include "settinguisoftkeylabellistdelegate.h"

#if IF_SUPPORT_SOFTKEY_LABEL
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "dsskeyuilogic/include/dsskeyuilogiccommon.h"

CSettinguiSoftkeyLabelListDelegate::CSettinguiSoftkeyLabelListDelegate()
{
}

CSettinguiSoftkeyLabelListDelegate::~CSettinguiSoftkeyLabelListDelegate()
{
}

CSettingUIDelegateBase * CSettinguiSoftkeyLabelListDelegate::CreateSoftkeyLabelListDelegate()
{
    CSettinguiSoftkeyLabelListDelegate * pDelegate = new CSettinguiSoftkeyLabelListDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettinguiSoftkeyLabelListDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        PageOpenData opPageData(kszPageIDDsskeySetting);
        DsskeyPageInfo m_PageInfo;
        m_PageInfo.m_eUIType = DSSKEY_UI_SOFTKEY;
        m_PageInfo.SetIndex(iDataIndex, 0);
        opPageData.pExtraData = (void*)(&m_PageInfo);
        g_SetUICallBackFun.EnterPage(opPageData);
        return true;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}
#endif
