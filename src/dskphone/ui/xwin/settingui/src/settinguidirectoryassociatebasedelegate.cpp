#include "settinguidirectoryassociatebasedelegate.h"

#if IF_SUPPORT_SELECT_CONTACT_INFO
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "translateiddefine.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "settingui/include/itemnamedefine.h"

namespace
{
#define SOFTKEY_THIRD_INDEX     2
#define SOFTKEY_INVALID_INDEX   -1
}

CDirectoryAssociateBaseDelegate::CDirectoryAssociateBaseDelegate()
{

}

CDirectoryAssociateBaseDelegate::~CDirectoryAssociateBaseDelegate()
{

}

CSettingUIDelegateBase* CDirectoryAssociateBaseDelegate::CreateDirectoryAssociateBaseDelegate()
{
    CDirectoryAssociateBaseDelegate* pDelegate = new CDirectoryAssociateBaseDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CDirectoryAssociateBaseDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() < MIN_SETTING_SOFTKEY_NUM)
    {
        return;
    }

    bool bShowDir = IsShowDirectorySoftkey();
    if (bShowDir)
    {
        vecSoftKey[SOFTKEY_THIRD_INDEX].m_iType = ST_DIRECTORY;
        vecSoftKey[SOFTKEY_THIRD_INDEX].m_strSoftkeyTitle = _LANG(TRID_DIRECTORY);
    }

    UpdateDeleteSoftkey(bShowDir);
}

bool CDirectoryAssociateBaseDelegate::IsShowDirectorySoftkey()
{
    return false;
}

void CDirectoryAssociateBaseDelegate::OnEditValueChange(int nIndex)
{
    g_SetUICallBackFun.RefreshSoftkey();
}

void CDirectoryAssociateBaseDelegate::UpdateDeleteSoftkey(bool bShowDelete)
{
    CSettingUIItemDataBase* pItemData = GetItemDataByDataIndex(GetFocusIndex());
    CSettingUILineEditItemData* pEditItemData = CSettingUILineEditItemData::GetLineEditDataItem(pItemData);
    if (NULL != pEditItemData)
    {
        pEditItemData->SetDeleteSoftkeyEnable(!bShowDelete);
        if (bShowDelete)
        {
            pEditItemData->SetDeleteSoftkeyIndex(SOFTKEY_INVALID_INDEX);
        }
        else
        {
            pEditItemData->SetDeleteSoftkeyIndex(SOFTKEY_THIRD_INDEX);
        }
    }
}

#endif
