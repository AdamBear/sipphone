#include "textmessagebasedelegate.h"
#include "settinguilogic/src/textmessagecontroller.h"
#include "settinguilogic/include/common.h"
#include "settinguitextmessageitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "messagebox/selectmessagebox.h"
#include "uicallbackfun.h"
#include "settingui/include/pageiddefine.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define MSGBOX_ID_DELETE        1
#define MSGBOX_ID_DELETE_ALL    2
#define NOTE_MSGBOX_SHOW_TIME   -1

typedef YLList<TextMsgItemSimpData> LIST_TEXT_MSG_ITEM_SIMP_DATA;
}

CTextMessageBaseDelegate::CTextMessageBaseDelegate(int nGType /*= TEXT_MSG_GROUP_TYPE_NONE*/)
    : m_nGType(nGType)
{

}

CTextMessageBaseDelegate::~CTextMessageBaseDelegate()
{

}

CSettingUIDelegateBase * CTextMessageBaseDelegate::CreatTextMessageBaseDelegate()
{
    CTextMessageBaseDelegate * pDelegate = new CTextMessageBaseDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CTextMessageBaseDelegate::LoadTextMessageBoxItem(CSettingUIPageDataBase * pPageData,
        int nGroupType)
{
    TextMsgSimpListData txtMsgList;
    if (NULL == g_pTextMessageController
            || !g_pTextMessageController->GetMessageList(&txtMsgList, nGroupType))
    {
        return;
    }

    LIST_TEXT_MSG_ITEM_SIMP_DATA::ListIterator iter = txtMsgList.m_listMessage.begin();
    LIST_TEXT_MSG_ITEM_SIMP_DATA::ListIterator listEnd  = txtMsgList.m_listMessage.end();

    for (int i = 0; iter != listEnd; ++iter, ++i)
    {
        const TextMsgItemSimpData & simpleData = (*iter);

        CSettingUITextMessageItemDataPtr pItemData = new CSettingUITextMessageItemData;
        if (NULL == pItemData)
        {
            return;
        }

        pItemData->SetDisplayName(simpleData.m_strName);
        pItemData->SetDateString(simpleData.m_strTime);
        pItemData->SetStatusIcon(simpleData.m_strIconPath);
        pItemData->SetShowIndex(false);
        pItemData->SetShowColon(false);
        yl::string strID = commonAPI_FormatString("%d", i);
        pItemData->SetItemID(strID);
        if (!pPageData->AddItem(pItemData))
        {
            delete pItemData;
        }
    }
}

bool CTextMessageBaseDelegate::GetOption(VEC_OPTION_ITEM_DATA & vecOption)
{
    ST_OPTION_ITEM_DATA stOption;

    stOption.m_nAction = ST_DELETE;
    stOption.m_strTitle = _LANG(TRID_DELETE);
    vecOption.push_back(stOption);

    stOption.m_nAction = ST_DELETEALL;
    stOption.m_strTitle = _LANG(TRID_DELETE_ALL);
    vecOption.push_back(stOption);
    return true;
}

CSettingUIPageDataBase * CTextMessageBaseDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL != m_pPageData && bReLoad)
    {
        LoadTextMessageBoxItem(m_pPageData, m_nGType);
    }

    return m_pPageData;
}

void CTextMessageBaseDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox || MESSAGEBOX_SELECTBOX != pMessageBox->GetType()
            || MessageBox_OK != pMessageBox->GetResult() || NULL == g_pTextMessageController)
    {
        return;
    }

    CSelectMessageBox * pOptionMsgBox = (CSelectMessageBox *)pMessageBox;

    int nAction = pOptionMsgBox->GetFcousAction();

    if (ST_DELETE == nAction)
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE),
                                               _LANG(TRID_IF_DELETE_SELECTED_MESSAGE)
                                               , SETTING_MESSAGEBOX_OkCancel, "", "", NOTE_MSGBOX_SHOW_TIME
                                               , MSGBOX_ID_DELETE);
    }
    else if (ST_DELETEALL == nAction)
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE),
                                               _LANG(TRID_IF_CLEAR_THIS_GROUP_MESSAGE)
                                               , SETTING_MESSAGEBOX_OkCancel, "", "", NOTE_MSGBOX_SHOW_TIME
                                               , MSGBOX_ID_DELETE_ALL);
    }
}

bool CTextMessageBaseDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_VIEW == iType)
    {
        TextMsgItemDetailData stDetailData;
        g_pTextMessageController->GetMessageDetail(&stDetailData, m_nGType, nDataIndex);
        g_pTextMessageController->SetTempTextMsgDetailData(stDetailData);
        g_pTextMessageController->SetTempDataIndex(nDataIndex);
        PageOpenData opPageData(kszPageIDTextMsgView);
        g_SetUICallBackFun.EnterPage(opPageData);
        SetRefreshDataWhenTopDelegate(true);
        return true;
    }
    else if (ST_BACK == iType)
    {
        // 界面退出需要清除缓存信息
        g_pTextMessageController->ResetTempTextMsgDetailData();
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

bool CTextMessageBaseDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /*= NULL*/)
{
    if (NULL == g_pTextMessageController || SETTING_MessageBox_OK != nResult)
    {
        return false;
    }

    if (MSGBOX_ID_DELETE == nID)
    {
        g_pTextMessageController->RemoveMessage(m_nGType, GetFocusIndex());
        g_SetUICallBackFun.RefreshUI(true);
    }
    else if (MSGBOX_ID_DELETE_ALL == nID)
    {
        g_pTextMessageController->RemoveAllMessage(m_nGType);
        g_SetUICallBackFun.RefreshUI(true);
    }

    return true;
}

void CTextMessageBaseDelegate::OnReturnToIdle()
{
    if (NULL == g_pTextMessageController)
    {
        return;
    }
    g_pTextMessageController->ResetTempTextMsgDetailData();
}

bool CTextMessageBaseDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    TextMsgItemDetailData stDetailData;
    //数字快捷键和ok都是打开信息，但对应的信息序号会有所不同
    if ((PHONE_KEY_0 <= nKeyCode && PHONE_KEY_9 >= nKeyCode) || PHONE_KEY_OK == nKeyCode)
    {
        //数字快捷键进入需要改变nTargeIndex为焦点位置的项
        if (PHONE_KEY_0 <= nKeyCode && PHONE_KEY_9 >= nKeyCode)
        {
            int nTotalNum = m_pPageData->GetTotalItemNumber();
            //数字1对应的信息index其实为0
            int nTargeIndex = nKeyCode - PHONE_KEY_0 - 1;
            if (nTargeIndex >= 0 && nTargeIndex < nTotalNum)
            {
                SetFocusIndex(nTargeIndex);
                g_SetUICallBackFun.RefreshUI(false);
                g_pTextMessageController->GetMessageDetail(&stDetailData, m_nGType, nTargeIndex);
                g_pTextMessageController->SetTempTextMsgDetailData(stDetailData);
                g_pTextMessageController->SetTempDataIndex(nTargeIndex);
            }
            else
            {
                nTargeIndex = nTotalNum - 1;
                SetFocusIndex(nTargeIndex);
                g_SetUICallBackFun.RefreshUI(false);
                return true;
            }

        }
        //按下ok键，焦点项还在本来的地方
        else if (PHONE_KEY_OK == nKeyCode)
        {
            g_pTextMessageController->GetMessageDetail(&stDetailData, m_nGType, nDataIndex);
            g_pTextMessageController->SetTempTextMsgDetailData(stDetailData);
            g_pTextMessageController->SetTempDataIndex(nDataIndex);
        }
        //如果是在草稿箱，则打开newmessage界面
        if (kszPageIDTextMsgDraftbox == m_strPageID)
        {
            PageOpenData opPageData(kszPageIDTextMsgNew);
            g_SetUICallBackFun.EnterPage(opPageData);
            SetRefreshDataWhenTopDelegate(true);
            return true;
        }
        //其他信箱打开为view界面
        else
        {
            PageOpenData opPageData(kszPageIDTextMsgView);
            g_SetUICallBackFun.EnterPage(opPageData);
            SetRefreshDataWhenTopDelegate(true);
            return true;
        }
    }

    return false;
}
