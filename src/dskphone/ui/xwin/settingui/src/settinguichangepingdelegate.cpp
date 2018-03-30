#include "settinguichangepingdelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "baseui/include/baseuicommon.h"
#include "keyguard/include/modkeyguard.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define INDEX_CURRENT_PIN_INVALID 0
#define INDEX_CONFIRM_PIN_INVALID 2
}

CSettinguiChangePingDelegate::CSettinguiChangePingDelegate()
    : m_bPwdVerified(false)
{

}

CSettinguiChangePingDelegate::~CSettinguiChangePingDelegate()
{

}

CSettingUIDelegateBase * CSettinguiChangePingDelegate::CreateChangePingDelegate()
{
    CSettinguiChangePingDelegate * pDelegate = new CSettinguiChangePingDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettinguiChangePingDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == m_pPageData || !pPageData->IsEnable())
    {
        return true;
    }

    CSettingUIDataValidInfo validInfo;
    yl::string strNewPin = GetValue(NEWPIN);
    if (!IsPageDataValid(validInfo, strNewPin))
    {
        g_SetUICallBackFun.DealWithValidData(validInfo, GetPageID());
        return true;
    }

    keyGuard_SaveUnlockPin(strNewPin);

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE);

    return true;
}

bool CSettinguiChangePingDelegate::IsPageDataValid(CSettingUIDataValidInfo & validInfo,
        const yl::string & strNewPin)
{
    if (!m_bPwdVerified)
    {
        yl::string strCurrentPin = GetValue(CURRENTPIN);

        if (!keyGuard_CheckUnlockPin(strCurrentPin))
        {
            validInfo.SetValidIndex(INDEX_CURRENT_PIN_INVALID);
            validInfo.SetValidNote(_LANG(TRID_CURRENT_PIN_IS_INCORRECT));
            return false;
        }
    }

    // 判断新密码的正确性
    yl::string strConfirmPin = GetValue(CONFIRMPIN);
    if (strNewPin != strConfirmPin)
    {
        validInfo.SetValidIndex(INDEX_CONFIRM_PIN_INVALID);
        validInfo.SetValidNote(_LANG(TRID_CONFIRM_PIN_MISMATCH));
        return false;
    }

    return true;
}

void CSettinguiChangePingDelegate::InitExtraData(void * pData)
{
    if (NULL == pData)
    {
        return;
    }
    m_bPwdVerified = *((bool *)pData);
}

bool CSettinguiChangePingDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    if (m_bPwdVerified)
    {
        SetItemShow(pPagedata, CURRENTPIN, false);
    }

    return bResult;
}
