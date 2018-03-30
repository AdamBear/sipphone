#include "textmessagedraftboxdelegate.h"

#include "settinguilogic/include/common.h"
#include "settinguitextmessageitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "translateiddefine.h"
#include "settingui/include/pageiddefine.h"

#include "settingui/include/modsettingui.h"
#include "settinguimanager.h"
#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/src/textmessagecontroller.h"
#include "baseui/include/modbaseui.h"
#include "uicallbackfun.h"

CTextMessageDraftBoxDelegate::CTextMessageDraftBoxDelegate(int nGType /*= TEXT_MSG_GROUP_DRAFT*/)
    : CTextMessageBaseDelegate(nGType)
{

}

CTextMessageDraftBoxDelegate::~CTextMessageDraftBoxDelegate()
{

}

CSettingUIDelegateBase * CTextMessageDraftBoxDelegate::CreatTextMessageDraftBoxDelegate()
{
    CTextMessageDraftBoxDelegate * pDelegate = new CTextMessageDraftBoxDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CTextMessageDraftBoxDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (m_pPageData->GetTotalItemNumber() > 0)
    {
        vecSoftKey[1].m_iType = ST_VIEW;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_VIEW);

        vecSoftKey[2].m_iType = ST_DELETE;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_DELETE);

        vecSoftKey[3].m_iType = ST_SEND;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SEND);
    }
}

bool CTextMessageDraftBoxDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_VIEW == iType || ST_SEND == iType)
    {
        if (NULL == g_pTextMessageController)
        {
            return false;
        }
        TextMsgItemDetailData stDetailData;
        g_pTextMessageController->GetMessageDetail(&stDetailData, m_nGType, nDataIndex);
        g_pTextMessageController->SetTempTextMsgDetailData(stDetailData);
        g_pTextMessageController->SetTempDataIndex(nDataIndex);
        SetRefreshDataWhenTopDelegate(true);
        PageOpenData opPageData(kszPageIDTextMsgNew);
        g_SetUICallBackFun.EnterPage(opPageData);
        return true;
    }
    else if (ST_DELETE == iType)
    {
        VEC_OPTION_ITEM_DATA vecOption;
        CSettingUIDelegateBase * pDelegateBase = g_pSettingUIManager->GetPageDelegate(
                    kszPageIDTextMsgDraftbox);
        CTextMessageDraftBoxDelegate * pDelegate = static_cast<CTextMessageDraftBoxDelegate *>
                (pDelegateBase);
        if (NULL == pDelegate || pDelegate != g_pSettingUIManager->GetTopDelegate()
                || !GetOption(vecOption))
        {
            return false;
        }

        BaseUI_ShowOptionMenu(pDelegate, vecOption);
        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}
