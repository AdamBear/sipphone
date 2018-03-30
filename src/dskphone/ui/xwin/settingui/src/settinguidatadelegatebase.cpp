#include "settinguidatadelegatebase.h"
#include <settinguilogic/include/settingdefine.h>
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "editcheckmethod.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "baseui/include/baseuicommon.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettinguiDataDelegateBase::CSettinguiDataDelegateBase()
{

}

CSettinguiDataDelegateBase::~CSettinguiDataDelegateBase()
{

}

CSettingUIDelegateBase * CSettinguiDataDelegateBase::CreateDataDelegate()
{
    CSettinguiDataDelegateBase * pDelegate = new CSettinguiDataDelegateBase;

    return (CSettingUIDelegateBasePtr)pDelegate;
}


bool CSettinguiDataDelegateBase::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /*= NULL*/)
{
    if ((MSGBOX_ID_SAVE == nID || MSGBOX_ID_SAVE_AND_REFRESH == nID)
            && SETTING_MessageBox_TimeOut == nResult)
    {
        g_SetUICallBackFun.FallBackToPrevousPage();

        if (MSGBOX_ID_SAVE_AND_REFRESH == nID)
        {
            g_SetUICallBackFun.RefreshUI(true);
        }
        return true;
    }

    return CSettingUIDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

void CSettinguiDataDelegateBase::SetValue(const yl::string & strItemId, const yl::string & strValue)
{
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(strItemId);
        if (NULL != pItemInfo)
        {
            pItemInfo->SetValue(strValue);
        }
    }
}

void CSettinguiDataDelegateBase::SetComboboxValue(const yl::string & strItemId, int nValue)
{
    if (NULL == m_pPageData)
    {
        return ;
    }

    CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(strItemId);
    if (NULL == pItemInfo)
    {
        return ;
    }

    char szBuffer[16];

    _snprintf(szBuffer, sizeof(szBuffer), "%d", nValue);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    yl::string strValue = szBuffer;

    pItemInfo->SetValue(strValue);
}

yl::string CSettinguiDataDelegateBase::GetValue(const yl::string & strItemId)
{
    yl::string strValue = "";
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(strItemId);
        if (NULL != pItemInfo)
        {
            strValue = pItemInfo->GetValue();
        }
    }

    return strValue;
}

int CSettinguiDataDelegateBase::GetComboboxValue(const yl::string & strItemId)
{
    yl::string strValue = "";
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(strItemId);
        if (NULL != pItemInfo)
        {
            strValue = pItemInfo->GetValue();
        }
    }

    if (strValue.empty() && !IsValidNumber(strValue))
    {
        return -1;
    }

    return atoi(strValue.c_str());
}

void CSettinguiDataDelegateBase::ShowNoteMessgaeBox(const yl::string & strNote, int nId,
        int nTime /*= NS_SET::g_SetPopNormalTime*/)
{
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), strNote,
                                           SETTING_MESSAGEBOX_NOTE, "", "", nTime, nId, NULL);
}

bool CSettinguiDataDelegateBase::IsPageOnOffCodeValid(const yl::string & strOn,
        const yl::string & strOff)
{
    if (!IsContentEmpty(strOn) && IsContentEmpty(strOff))
    {
        return false;
    }

    return true;
}
