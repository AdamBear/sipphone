#include "zeroredirectdelegate.h"
#include "settinguilogic/include/settingdefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/settinguidialogmanager.h"
#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/dataloader.h"
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/include/zero_common.h"
#include "common/common_data_define.h"
#include "translateiddefine.h"

#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "settingui/src/uicallbackfun.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "zerotouchui/include/modzerotouchui.h"
#include "uimanager/moduimanager.h"

namespace
{
#define   SOFT_KEY_BACK_INDEX   0
#define   SOFT_KEY_OK_INDEX     3
}

CZeroRedirectDelegate::CZeroRedirectDelegate()
    : CSettingUIDelegateBase()
{

}

CZeroRedirectDelegate::~CZeroRedirectDelegate()
{
    UIManager_LockReturnToIdle(false);
}

CSettingUIDelegateBase * CZeroRedirectDelegate::CreateZeroRedirectDelegate()
{
    CZeroRedirectDelegate * pStatusDelegate = new CZeroRedirectDelegate;

    return (CSettingUIDelegateBasePtr)pStatusDelegate;
}

void CZeroRedirectDelegate::GetValueById(const yl::string & strItemId, yl::string & strValue)
{
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(strItemId);
        if (NULL != pItemInfo)
        {
            strValue = pItemInfo->GetValue();
        }
    }
}

void CZeroRedirectDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    vecSoftKey[0].m_iType = ST_BACK;
    vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);

    vecSoftKey[3].m_iType = ST_NEXT;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_OK);
}

bool CZeroRedirectDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (!bLongPress)
    {
        if (ST_NEXT == iType)
        {
            OnStartRedirect();
            return true;
        }
        else if (ST_BACK == iType)
        {
            ZeroTouchUI_Destroy();
            return true;
        }
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

bool CZeroRedirectDelegate::IsRejectReturnToIdle()
{
    return false;
}

void CZeroRedirectDelegate::InitExtraData(void * pData)
{
    UIManager_LockReturnToIdle(true);
}

bool CZeroRedirectDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (PHONE_KEY_OK == nKeyCode && bPress)
    {
        OnStartRedirect();
        return true;
    }
    return false;
}

void CZeroRedirectDelegate::OnStartRedirect()
{
    yl::string strUserName;
    yl::string strPwd;
    GetValueById(REDIRECT_USERNAME, strUserName);
    GetValueById(REDIRECT_PSW, strPwd);

    g_SetUICallBackFun.DropOutSettingUI();
    SettingUILogic_SetRedirectData(strUserName, strPwd);
    // 检查网络,进行重定向操作
    if (SettingUILogic_CheckingNetwork())
    {
        // liuqy TBD AutoControll接口替换
        //GetAutopController()->ReadyRedirect();
    }
}

