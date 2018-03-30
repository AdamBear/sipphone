#include "textmessagesentboxdelegate.h"
#include "settinguilogic/src/textmessagecontroller.h"
#include "settinguilogic/include/common.h"
#include "settinguitextmessageitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "translateiddefine.h"
#include "settingui/include/pageiddefine.h"

#include "languagehelper/modlangtranslate.h"
#include "baseui/include/modbaseui.h"
#include "settinguimanager.h"
#include "settinguilogic/src/textmessagecontroller.h"
#include "uicallbackfun.h"

CTextMessageSentBoxDelegate::CTextMessageSentBoxDelegate(int nGType /*= TEXT_MSG_GROUP_SENT*/)
    : CTextMessageBaseDelegate(nGType)
{

}

CTextMessageSentBoxDelegate::~CTextMessageSentBoxDelegate()
{

}

CSettingUIDelegateBase * CTextMessageSentBoxDelegate::CreatTextMessageSentBoxDelegate()
{
    CTextMessageSentBoxDelegate * pDelegate = new CTextMessageSentBoxDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CTextMessageSentBoxDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (m_pPageData->GetTotalItemNumber() > 0)
    {
        vecSoftKey[2].m_iType = ST_DELETE;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_DELETE);

        vecSoftKey[3].m_iType = ST_VIEW;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_VIEW);
    }
}

bool CTextMessageSentBoxDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_DELETE == iType)
    {
        VEC_OPTION_ITEM_DATA vecOption;
        CSettingUIDelegateBase * pDelegateBase = g_pSettingUIManager->GetPageDelegate(
                    kszPageIDTextMsgSentbox);
        CTextMessageSentBoxDelegate * pDelegate = static_cast<CTextMessageSentBoxDelegate *>(pDelegateBase);
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
