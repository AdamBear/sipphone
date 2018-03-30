#include "settinguipaginglistdelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "baseui/include/baseuicommon.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguimenuitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "pageiddefine.h"
#include "settinguimanager.h"
#include "settingactiontypemgr.h"
#include "settinguilogic/include/common.h"
#include "feature/include/modfeature.h"
#include "settinguilogic/src/featurecontroller.h"
#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "messagebox/selectmessagebox.h"
#include "baseui/include/modbaseui.h"

namespace
{
#define MSGBOX_ID_LOAD_FAILED 0
#define MSGBOX_ID_DELETE 1
#define MSGBOX_ID_DELETE_ALL 2
#define NOTE_MSGBOX_SHOW_TIME 1500
}

CSettinguiPagingListDelegate::CSettinguiPagingListDelegate()
{
    feature_MutliPaingCallBack(OnPagingListCallBack, true);
}

CSettinguiPagingListDelegate::~CSettinguiPagingListDelegate()
{
    feature_MutliPaingCallBack(OnPagingListCallBack, true);
}

CSettingUIDelegateBase * CSettinguiPagingListDelegate::CreatePagingListDelegate()
{
    CSettinguiPagingListDelegate * pDelegate = new CSettinguiPagingListDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CSettinguiPagingListDelegate::OnPagingListCallBack()
{
    CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDPagingList);
    if (NULL != pDelegate)
    {
        g_SetUICallBackFun.RefreshUI(true, kszPageIDPagingList);
    }
}

bool CSettinguiPagingListDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /* = NULL */)
{
    if (MSGBOX_ID_DELETE == nID || MSGBOX_ID_DELETE_ALL == nID)
    {
        if (SETTING_MessageBox_OK == nResult)
        {
            bool bDelete = false;
            if (MSGBOX_ID_DELETE == nID)
            {
                bDelete = feature_MutliPaingDeleteGroupInfo(m_pPageData->GetFocusIndex() + 1);
            }
            else
            {
                bDelete = feature_MutliPaingDeleteAll();
            }

            if (bDelete)
            {
                g_SetUICallBackFun.RefreshUI(true, kszPageIDPagingList);
            }
            else
            {
                ShowNoteMessgaeBox(_LANG(TRID_DELETE_PAGING_FAILED));
            }
        }

        return true;
    }

    return CSettingUIDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

CSettingUIPageDataBase * CSettinguiPagingListDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        LoadPagingListItem(m_pPageData);
    }

    return m_pPageData;
}

void CSettinguiPagingListDelegate::LoadPagingListItem(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return ;
    }

    // 获取显示的数据
    m_pagelist.clear();
    if (!feature_MutliPaingGetList(m_pagelist))
    {
        ShowNoteMessgaeBox(_LANG(TRID_LOAD_FAILED));
        return;
    }

    // 显示数据
    int nSize = m_pagelist.size();
    if (nSize > 0)
    {
        for (int i = 0; i <  nSize; ++i)
        {
            yl::string strItemID = commonAPI_FormatString("%d", m_pagelist[i].iIndex);
            yl::string strText = strItemID + ". ";
            if (m_pagelist[i].strAddress.empty())
            {
                strText += _LANG(TRID_ACCOUNT_EMPTY);
            }
            else if (m_pagelist[i].strLabel.empty())
            {
                strText += m_pagelist[i].strAddress;
            }
            else
            {
                strText += m_pagelist[i].strLabel;
            }

            AddPagingListItem(strText, pPageData, strItemID);
        }
    }
    else
    {
        AddMenuItem(pPageData, "", _LANG(TRID_NONE));
    }
}

bool CSettinguiPagingListDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_OPTION == eActionType)
    {
        VEC_OPTION_ITEM_DATA vecOption;
        ST_OPTION_ITEM_DATA stOption;

        stOption.m_nAction = OPTION_EDIT;
        stOption.m_strTitle = _LANG(TRID_EDIT);
        vecOption.push_back(stOption);

        stOption.m_nAction = OPTION_DELETE;
        stOption.m_strTitle = _LANG(TRID_DELETE);
        vecOption.push_back(stOption);

        stOption.m_nAction = OPTION_DELETE_ALL;
        stOption.m_strTitle = _LANG(TRID_DELETE_ALL);
        vecOption.push_back(stOption);
        BaseUI_ShowOptionMenu(this, vecOption);
        return true;
    }
    else if (SET_ACTION_TYPE_EDIT == eActionType)
    {
        OnEditClicked();
        return true;
    }
    else if (SET_ACTION_TYPE_DELETE == eActionType)
    {
        OnDeleteClicked();
        return true;
    }
    else if (SET_ACTION_TYPE_DELETEALL == eActionType)
    {
        OnDeleteAllClicked();
        return true;
    }
    else if (SET_ACTION_TYPE_PAGING == eActionType || SET_ACTION_TYPE_CLICK == eActionType)
    {
        int iIndex  =  m_pPageData->GetFocusIndex();
        if (NULL == m_pPageData || m_pagelist.size() <= iIndex)
        {
            return false;
        }
        OnPagingClicked(iIndex + 1);
        return true;
    }
    else if (SET_ACTION_TYPE_CANCLE == eActionType)
    {
        g_SetUICallBackFun.RefreshSoftkey(GetPageID());
        return true;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

void CSettinguiPagingListDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & objSoftkey)
{

    int nSize = m_pagelist.size();

    if (SOFTKEY_NUM != objSoftkey.size())
    {
        return;
    }

    objSoftkey[0].m_iType = ST_BACK;
    objSoftkey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);

    if (nSize <= 0)
    {
        for (int i = 0; i < SOFTKEY_NUM; ++i)
        {
            if (0 != i)
            {
                objSoftkey[i].m_iType = ST_EMPTY;
                objSoftkey[i].m_strSoftkeyTitle = "";
            }
        }
    }
    else
    {
        objSoftkey[1].m_iType = ST_EMPTY;
        objSoftkey[1].m_strSoftkeyTitle = "";
        if (feature_UserModeConfigItemReadOnly(kszPageIDPagingList))
        {
            objSoftkey[2].m_iType = ST_ENTER;
            objSoftkey[2].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        }
        else
        {
            objSoftkey[2].m_iType = ST_OPTION;
            objSoftkey[2].m_strSoftkeyTitle = _LANG(TRID_OPTION);
        }
        objSoftkey[3].m_iType = ST_PAGING;
        objSoftkey[3].m_strSoftkeyTitle = _LANG(TRID_SOFTKEY_PAGING);
    }
}

void CSettinguiPagingListDelegate::MessageBoxCallBack(CMessageBoxBase* pMessageBox)
{
    if (NULL == pMessageBox || MESSAGEBOX_SELECTBOX != pMessageBox->GetType()
            || MessageBox_OK != pMessageBox->GetResult())
    {
        return;
    }

    CSelectMessageBox* pOptionMsgBox = (CSelectMessageBox*)pMessageBox;
    if (NULL == pOptionMsgBox)
    {
        return;
    }

    int nAction = pOptionMsgBox->GetFcousAction();
    switch (nAction)
    {
    case OPTION_EDIT:
        {
            OnEditClicked();
        }
        break;
    case OPTION_DELETE:
        {
            OnDeleteClicked();
        }
        break;
    case OPTION_DELETE_ALL:
        {
            OnDeleteAllClicked();
        }
        break;
    default:
        break;
    }
}

void CSettinguiPagingListDelegate::AddPagingListItem(const yl::string & strName,
        CSettingUIPageDataBase * pPageData, const yl::string strID)
{
    if (NULL == pPageData)
    {
        return;
    }

    AddMenuItem(pPageData, strID, strName, "", false);
}

bool CSettinguiPagingListDelegate::OnPagingClicked(int iIndex)
{
    //序号iIndex从1开始
    if (iIndex < 1 || iIndex > m_pagelist.size() || m_pagelist[iIndex - 1].strAddress.empty())
    {
        SETTINGUI_INFO("When Paging to group %d, address is empty!", iIndex);
        return false;
    }

    char szChannel[16];

    _snprintf(szChannel, sizeof(szChannel), "%d", m_pagelist[iIndex - 1].iChannel);
    szChannel[sizeof(szChannel) - 1] = '\0';

    if (talklogic_CallPagingOut(m_pagelist[iIndex - 1].strLabel.c_str(),
                                m_pagelist[iIndex - 1].strAddress.c_str(), -1, szChannel))
    {
        return true;
    }

    ShowNoteMessgaeBox(_LANG(TRID_PAING_FAILED));
    return false;
}

void CSettinguiPagingListDelegate::OnEditClicked()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    int iIndex = m_pPageData->GetFocusIndex();
    if (m_pagelist.size() <= iIndex)
    {
        return;
    }

    PageOpenData opPageData(kszPageIDPagingListEdit);
    opPageData.pExtraData = (void *) & (m_pagelist[iIndex].iIndex);
    g_SetUICallBackFun.EnterPage(opPageData);

}

void CSettinguiPagingListDelegate::OnDeleteClicked()
{
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING), _LANG(TRID_DELETE_PAGING)
                                           , SETTING_MESSAGEBOX_OkCancel, "", "", -1
                                           , MSGBOX_ID_DELETE, NULL);
}

void CSettinguiPagingListDelegate::OnDeleteAllClicked()
{
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING),
                                           _LANG(TRID_DELETE_ALL_PAGING)
                                           , SETTING_MESSAGEBOX_OkCancel, "", "", -1
                                           , MSGBOX_ID_DELETE_ALL, NULL);
}

void CSettinguiPagingListDelegate::ShowNoteMessgaeBox(const yl::string & strNote)
{
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, "", strNote, SETTING_MESSAGEBOX_NOTE, "", "",
                                           NOTE_MSGBOX_SHOW_TIME);
}
