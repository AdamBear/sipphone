#include "messagelistdelegate.h"
#include <settinguilogic/include/settingdefine.h>
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/include/common.h"

#include "settingui/include/itemnamedefine.h"

#include "common/common_data_define.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CMessageListDelegate::CMessageListDelegate()
    : CSettingUIDelegateBase()
{
}

CMessageListDelegate::~CMessageListDelegate()
{
}

CSettingUIDelegateBase* CMessageListDelegate::CreateStatusDelegate()
{
    CMessageListDelegate* pStatusDelegate = new CMessageListDelegate;

    return (CSettingUIDelegateBasePtr)pStatusDelegate;
}

bool CMessageListDelegate::PageDataFilter(CSettingUIPageDataBase* pPagedata,
        bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

#if !IF_FEATURE_METASWITCH_VOICEMAIL
    SetItemHide(pPagedata, MESW_VOICE_MAIL);
#endif
    return bResult;
}

void CMessageListDelegate::SetItemHide(CSettingUIPageDataBase* pPagedata,
                                       const yl::string& strItemId)
{
    if (NULL != pPagedata)
    {
        CSettingUIItemDataBasePtr pItemData = pPagedata->GetItemByItemId(strItemId);
        if (NULL != pItemData)
        {
            pItemData->SetShow(false);
        }
    }
}
