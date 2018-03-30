#include "edkpromptdelegate.h"
#include "settingui/include/setuidatadefine.h"
#include "settingui/include/itemnamedefine.h"
#include "translateiddefine.h"
#include "settingui/include/pageiddefine.h"

#include "messagebox/modmessagebox.h"
#include "languagehelper/modlangtranslate.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "uicallbackfun.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "edk/include/modedk.h"

namespace
{
#define IME_DEFAULT_TEXT                    "name"
#define IME_DEFAULT_NUMBER                  "number"
#define IME_DEFAULT_abc                     "abc"
#define IME_PASSWORD                        "pwd"
#define IME_DEFAULT_PWD_abc                 "abc_PWD"
}

CEdkPromptDelegate::CEdkPromptDelegate()
{

}

CEdkPromptDelegate::~CEdkPromptDelegate()
{
}

CSettingUIDelegateBase * CEdkPromptDelegate::CreateEdkPromptDelegate()
{
    CEdkPromptDelegate * pDelegate = new CEdkPromptDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CEdkPromptDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
                                      bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData)
    {
        return true;
    }

    yl::string strPrompt = GetValue(ITEMID_EDK_PROMPT);

    if (strPrompt.empty())
    {
        // 弹出提示
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING),
                                               _LANG(TRID_EDK_PROMFT_EMPTY)
                                               , SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
    }
    else
    {
        // 输入正确，并反馈给逻辑层
        FeedBack(true);
    }

    return true;
}

bool CEdkPromptDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        eActionType = SET_ACTION_TYPE_SAVE;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

// 反馈数据给逻辑层
void CEdkPromptDelegate::FeedBack(bool bOk/* = false*/)
{
    FuncEdkActionCallBack pCallBackFun = m_stEdkData.pCallBackFun;
    int iId = m_stEdkData.iId;
    yl::string strPrompt = GetValue(ITEMID_EDK_PROMPT);

    // 退出当前界面
    g_SetUICallBackFun.FallBackToPrevousPage();

    // 反馈数据给逻辑层
    if (NULL != pCallBackFun)
    {
        pCallBackFun(bOk, iId, strPrompt);
    }
}

bool CEdkPromptDelegate::OnBackAction()
{
    // 取消执行，并反馈给逻辑层
    FeedBack(false);
    return true;
}

void CEdkPromptDelegate::InitExtraData(void * pData)
{
    if (NULL == pData)
    {
        return;
    }

    m_stEdkData = *((ST_EDK_DATA *)pData);
}

CSettingUIPageDataBase * CEdkPromptDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData)
    {
        return NULL;
    }

    CSettingUILineEditItemDataPtr pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(
                m_pPageData->GetItemByItemId(ITEMID_EDK_PROMPT));
    if (NULL == pEditItem)
    {
        return NULL;
    }

    // 根据输入框的索引调用逻辑层获取输入框的信息
    edk_prompt_data_t stPromptData;
    Edk_GetPromptByIndex(m_stEdkData.iIndex, stPromptData);

    if (!stPromptData.m_strTitle.empty())
    {
        m_pPageData->SetPageTitle(stPromptData.m_strTitle);
    }

    if (stPromptData.m_strLabel.empty())
    {
        pEditItem->SetDisplayName(_LANG(TRID_DEFAULT));
    }
    else
    {
        pEditItem->SetDisplayName(stPromptData.m_strLabel);
    }
    pEditItem->SetMaxLength(stPromptData.m_iMaxInput);
    if (EDK_INPUT_NUMERIC == stPromptData.m_eInputType)
    {
        pEditItem->SetIME(IME_DEFAULT_NUMBER);
    }
    else
    {
        if (stPromptData.m_bMasked)
        {
            pEditItem->SetIME(IME_PASSWORD);
            pEditItem->SetDefaultIME(IME_DEFAULT_PWD_abc);
        }
        else
        {
            pEditItem->SetIME(IME_DEFAULT_TEXT);
            pEditItem->SetDefaultIME(IME_DEFAULT_abc);
        }
    }
    pEditItem->SetPassword(stPromptData.m_bMasked);

    return m_pPageData;
}
