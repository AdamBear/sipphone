#include "textmessageinboxdelegate.h"

#include "settinguilogic/include/common.h"
#include "settinguitextmessageitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "translateiddefine.h"
#include "settingui/include/pageiddefine.h"

#include "languagehelper/modlangtranslate.h"
#include "settingui/include/modsettingui.h"
#include "baseui/include/modbaseui.h"
#include "settinguimanager.h"
#include "settinguilogic/src/textmessagecontroller.h"
#include "uicallbackfun.h"


CTextMessageInboxDelegate::CTextMessageInboxDelegate(int nGType /*= TEXT_MSG_GROUP_IN*/)
    : CTextMessageBaseDelegate(nGType)
{

}

CTextMessageInboxDelegate::~CTextMessageInboxDelegate()
{

}

CSettingUIDelegateBase * CTextMessageInboxDelegate::CreatTextMessageInboxDelegate()
{
    CTextMessageInboxDelegate * pDelegate = new CTextMessageInboxDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CTextMessageInboxDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (m_pPageData->GetTotalItemNumber() > 0)
    {
        vecSoftKey[1].m_iType = ST_REPLY;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_REPLAY);

        vecSoftKey[2].m_iType = ST_DELETE;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_DELETE);

        vecSoftKey[3].m_iType = ST_VIEW;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_VIEW);
    }
}

bool CTextMessageInboxDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (NULL == g_pTextMessageController)
    {
        return false;
    }

    if (ST_REPLY == iType)
    {
        TextMsgItemDetailData stDetailData;
        g_pTextMessageController->GetMessageDetail(&stDetailData, m_nGType, nDataIndex);
        g_pTextMessageController->SetTempTextMsgDetailData(stDetailData);
        PageOpenData opPageData(kszPageIDTextMsgNew);
        g_SetUICallBackFun.EnterPage(opPageData);
        return true;
    }
    else if (ST_DELETE == iType)
    {
        VEC_OPTION_ITEM_DATA vecOption;
        CSettingUIDelegateBase * pDelegateBase = g_pSettingUIManager->GetPageDelegate(
                    kszPageIDTextMsgInbox);
        CTextMessageInboxDelegate * pDelegate = static_cast<CTextMessageInboxDelegate *>(pDelegateBase);
        if (NULL == pDelegate || pDelegate != g_pSettingUIManager->GetTopDelegate()
                || !GetOption(vecOption))
        {
            return false;
        }

        BaseUI_ShowOptionMenu(pDelegate, vecOption);
        return true;
    }

    return CTextMessageBaseDelegate::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

