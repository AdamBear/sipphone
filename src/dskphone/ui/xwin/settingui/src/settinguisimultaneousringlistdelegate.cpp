#include "settinguisimultaneousringlistdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_PRIORITY_ALERT

#include "settinguimanager.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "common/common_data_define.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "feature/include/modfeature.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "messagebox/selectmessagebox.h"
#include "baseui/include/modbaseui.h"

namespace
{
#define MAX_LOCATION 10
}

CSettinguiSimultaneousRingListDelegate::CSettinguiSimultaneousRingListDelegate()
    : m_bLocationListCanEidit(true)
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiSimultaneousRingListDelegate::OnMessage);
}

CSettinguiSimultaneousRingListDelegate::~CSettinguiSimultaneousRingListDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiSimultaneousRingListDelegate::OnMessage);
}

CSettingUIDelegateBase *
CSettinguiSimultaneousRingListDelegate::CreateSimultaneousRingListDelegate()
{
    CSettinguiSimultaneousRingListDelegate * pDelegate = new CSettinguiSimultaneousRingListDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

//对外提供的消息（XSI_MESSAGE_REQ_FAIL和XSI_MESSAGE_REQ_FINISH）处理函数
LRESULT CSettinguiSimultaneousRingListDelegate::OnMessage(msgObject & msg)
{
    CSettinguiSimultaneousRingListDelegate * pDelegate = (CSettinguiSimultaneousRingListDelegate *)
            g_pSettingUIManager->GetPageDelegate(kszPageIDCallControllSimultaneousRingList);
    if (NULL == pDelegate || pDelegate != g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    switch (msg.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            pDelegate->OnMessageReqFinish(msg);
            return TRUE;
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            pDelegate->OnMessageReqFail(msg);
            return TRUE;
        }
        break;
    default:
        break;
    }

    return FALSE;
}

void CSettinguiSimultaneousRingListDelegate::OnMessageReqFinish(msgObject & msg)
{
    if (XSI_SIMULTANEOUSRING_PUT == msg.wParam)
    {
        MessageBox_RemoveMessageBox(this, MessageBox_Destory, m_eMsgboxID);

        bool bReload = XSI_MSGBOX_ID_DELETEING == m_eMsgboxID;

        g_SetUICallBackFun.RefreshUI(bReload, GetPageID());
    }
    else if (XSI_SIMULTANEOUSRING_GET == msg.wParam)
    {
        OnReturnLocationList();
    }
}

void CSettinguiSimultaneousRingListDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nResult = pMessageBox->GetResult();
    int nType = pMessageBox->GetType();
    if (MESSAGEBOX_SELECTBOX == nType && MessageBox_OK == nResult)
    {
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
                int nDataIndex = GetFocusIndex();
                //进入edit location界面
                OnEnterPage(kszPageIDCallControllSimultaneousRingEdit, GetItemRealIndex(nDataIndex));
            }
            break;
        case OPTION_DELETE:
            {
                ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_IF_DELETE_THE_NUMBER), XSI_MSGBOX_ID_DELETE, -1,
                               MESSAGEBOX_OkCancel);
            }
            break;
        case OPTION_DELETE_ALL:
            {
                ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_IF_DELETE_ALL_NUMBERS), XSI_MSGBOX_ID_DELETE_ALL, -1,
                               MESSAGEBOX_OkCancel);
            }
            break;
        default:
            break;
        }
        return;
    }

    int nID = pMessageBox->GetID();

    if (MessageBox_OK == nResult && (XSI_MSGBOX_ID_DELETE == nID || XSI_MSGBOX_ID_DELETE_ALL == nID))
    {
        bool bDeleteAll = (XSI_MSGBOX_ID_DELETE == nID) ? false : true;
        OnLocationDelete(bDeleteAll);
        return;
    }
    else if (XSI_MSGBOX_ID_SAVE == nID)
    {
        //Location List 界面保存弹窗退出不返回上一级界面
        return;
    }
    else if (XSI_MSGBOX_ID_ADD == nID && MessageBox_TimeOut == nResult)
    {
        OnEnterPage(kszPageIDCallControllSimultaneousRingAdd, 1);
        return;
    }

    CSettinguiXSIDelegatebase::MessageBoxCallBack(pMessageBox);
}

void CSettinguiSimultaneousRingListDelegate::InitExtraData(void * pData)
{
    CSettingUIAccountDelegateBase::InitExtraData(pData);

    m_bLocationListCanEidit = !feature_UserModeConfigItemReadOnly(
                                  kszPageIDCallControllSimultaneousRingList);
}

CSettingUIPageDataBase * CSettinguiSimultaneousRingListDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && IsHasAccountRegister())
    {
        m_eMsgboxID = XSI_MSGBOX_ID_LOADING;
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), m_eMsgboxID, -1,
                       MESSAGEBOX_Cancel);

        SRP_QuerySimultaneousRing(m_iLineID);
    }

    return m_pPageData;
}

bool CSettinguiSimultaneousRingListDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    bool bActive = ("0" == GetValue(SIMULTANEOUSRING_STATUS)) ? false : true;

    if (bActive && m_simRingInfo.simRingLocationArry.size() < 1)
    {
        m_eMsgboxID = XSI_MSGBOX_ID_ADD;
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_NEED_A_NUMBER), XSI_MSGBOX_ID_ADD, 1500,
                       MESSAGEBOX_NOTE);
        return false;
    }
    else
    {
        m_simRingInfo.bActive = bActive;
        SRP_SetSimultaneousRing(m_simRingInfo);

        m_eMsgboxID = XSI_MSGBOX_ID_SAVE;
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), XSI_MSGBOX_ID_SAVE, -1,
                       MESSAGEBOX_Cancel);

        // http://10.3.5.199/Bug.php?BugID=64594
        etlSleep(100);
    }

    return true;
}

void CSettinguiSimultaneousRingListDelegate::OnEnterPage(const yl::string & strPage, int iDataIndex)
{
    PageOpenData opPageData(strPage);
    opPageData.pExtraData = (void *)&iDataIndex;
    g_SetUICallBackFun.EnterPage(opPageData);

    SetRefreshDataWhenTopDelegate(true);
}

bool CSettinguiSimultaneousRingListDelegate::OnPageAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        if (!m_bLocationListCanEidit && m_simRingInfo.simRingLocationArry.size() > 0 && iDataIndex > 0)
        {
            int nRealIndex = GetItemRealIndex(iDataIndex);
            OnEnterPage(kszPageIDCallControllSimultaneousRingEdit, nRealIndex);
        }
        else if (m_bLocationListCanEidit)
        {
            SavePageData(m_pPageData);
        }

        return true;
    }

    return CSettinguiXSIDelegatebase::OnPageAction(eActionType, iDataIndex);
}

bool CSettinguiSimultaneousRingListDelegate::OnSoftkeyAction(int iType, bool bLongPress,
        int nDataIndex)
{
    if (ST_OPTION == iType)
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
    }
    else if (ST_ENTER == iType && !m_bLocationListCanEidit && m_simRingInfo.simRingLocationArry.size() > 0)
    {
        if (nDataIndex > 0)
        {
            OnEnterPage(kszPageIDCallControllSimultaneousRingEdit, GetItemRealIndex(nDataIndex));
        }
    }
    else if (ST_ADD == iType)
    {
        if (IsEnableAddLocation())
        {
            OnEnterPage(kszPageIDCallControllSimultaneousRingAdd, nDataIndex);
        }
    }
    else if (ST_DELETEALL == iType)
    {
        m_eMsgboxID = XSI_MSGBOX_ID_DELETE_ALL;
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_IF_DELETE_ALL_NUMBERS), m_eMsgboxID, -1,
                       MESSAGEBOX_OkCancel);
    }
    else
    {
        return CSettingUIAccountDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
    }

    return true;
}

bool CSettinguiSimultaneousRingListDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (PHONE_KEY_0 <= nKeyCode && nKeyCode <= PHONE_KEY_9 && NULL != m_pPageData && bPress)
    {
        int nTotalNum = m_pPageData->GetTotalItemNumber();
        if (PHONE_KEY_0 == nKeyCode)
        {
            int nEndIndex = nTotalNum - 1;
            if (nEndIndex > 0 && nDataIndex != nEndIndex)
            {
                SetFocusIndex(nEndIndex);
                g_SetUICallBackFun.RefreshUI(false);
            }
        }
        else
        {
            int nTargeIndex = nKeyCode - PHONE_KEY_0;
            if (nTargeIndex != nDataIndex && nTargeIndex < nTotalNum)
            {
                SetFocusIndex(nTargeIndex);
                g_SetUICallBackFun.RefreshUI(false);
            }
        }

        return true;
    }

    return CSettinguiXSIDelegatebase::OnKeyEvent(nKeyCode, bPress, nDataIndex);
}

void CSettinguiSimultaneousRingListDelegate::OnCancleClick()
{
    SRP_CancelRequest();
}

// 加载Location List成功后，获取List
void CSettinguiSimultaneousRingListDelegate::OnReturnLocationList()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    // 取消提示框
    MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_LOADING);

    //获取Location数据
    m_simRingInfo.Reset();
    SRP_GetSimultaneousRingInfo(m_simRingInfo);
    SetComboboxValue(SIMULTANEOUSRING_STATUS, m_simRingInfo.bActive);

    m_pPageData->DelFromItem(SIMULTANEOUSRING_STATUS);
    for (int i = 0; i < m_simRingInfo.simRingLocationArry.size(); ++i)
    {
        CSettingUIItemDataBasePtr pItemData = new CSettingUIItemDataBase;
        if (NULL != pItemData)
        {
            yl::string strItemID = commonAPI_FormatString("%d", i);
            yl::string strName = commonAPI_FormatString("%d. ", i + 1);
            strName += m_simRingInfo.simRingLocationArry.at(i).strAddress;
            pItemData->SetShowIndex(false);
            pItemData->SetItemID(strItemID);
            pItemData->SetDisplayName(strName);
            pItemData->SetShowColon(false);

            if (!m_pPageData->AddItem(pItemData))
            {
                delete pItemData;
            }

        }
    }

    g_SetUICallBackFun.RefreshUI(false, GetPageID());
}

void CSettinguiSimultaneousRingListDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData &
        vecSoftKey)
{
    if (SOFTKEY_NUM != vecSoftKey.size())
    {
        return;
    }

    if (GetItemDataIndexByItemId(SIMULTANEOUSRING_STATUS) == GetFocusIndex())
    {
        vecSoftKey[0].m_iType = ST_BACK;
        vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);
        if (m_bLocationListCanEidit)
        {
            vecSoftKey[1].m_iType = ST_ADD;
            vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ADD);
            vecSoftKey[2].m_iType = ST_DELETEALL;
            vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_DELETE_ALL);
            vecSoftKey[3].m_iType = ST_SAVE;
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SAVE);
        }
        else
        {
            vecSoftKey[1].m_iType = ST_EMPTY;
            vecSoftKey[1].m_strSoftkeyTitle = "";
            vecSoftKey[2].m_iType = ST_EMPTY;
            vecSoftKey[2].m_strSoftkeyTitle = "";
            vecSoftKey[3].m_iType = ST_EMPTY;
            vecSoftKey[3].m_strSoftkeyTitle = "";
        }
        return;
    }

    vecSoftKey[0].m_iType = ST_BACK;
    vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);

    if (m_bLocationListCanEidit)
    {
        vecSoftKey[1].m_iType = ST_ADD;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ADD);
        vecSoftKey[3].m_iType = ST_SAVE;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SAVE);
    }
    else
    {
        vecSoftKey[1].m_iType = ST_EMPTY;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_EMPTY);
        vecSoftKey[3].m_iType = ST_ENTER;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
    }

    if (m_bLocationListCanEidit && m_simRingInfo.simRingLocationArry.size() > 0)
    {
        vecSoftKey[2].m_iType = ST_OPTION;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_OPTION);
    }
    else
    {
        vecSoftKey[2].m_iType = ST_EMPTY;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_EMPTY);
    }
}

void CSettinguiSimultaneousRingListDelegate::OnLocationDelete(bool bDeleteAll)
{
    bool bDelete = false;

    if (!bDeleteAll)
    {
        //删除单个Location
        int nFocusIndex = GetFocusIndex();
        if (nFocusIndex != GetItemDataIndexByItemId(SIMULTANEOUSRING_STATUS))
        {
            int nRealIndex = GetItemRealIndex(nFocusIndex);
            if (nRealIndex >= 0 && nRealIndex < m_simRingInfo.simRingLocationArry.size())
            {
                m_simRingInfo.simRingLocationArry.removeAt(nRealIndex);
                SRP_SetSimultaneousRing(m_simRingInfo);
                bDelete = true;
            }
        }
    }
    else
    {
        // 删除全部Location
        m_simRingInfo.simRingLocationArry.clear();
        m_simRingInfo.bActive = false;
        // 执行操作
        SRP_SetSimultaneousRing(m_simRingInfo);

        bDelete = true;
    }

    if (bDelete)
    {
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_DELETING_PLEASE_WAIT), XSI_MSGBOX_ID_DELETEING, -1,
                       MESSAGEBOX_Cancel);
        m_eMsgboxID = XSI_MSGBOX_ID_DELETEING;
    }
}

//返回是否可以增加新的Location
bool CSettinguiSimultaneousRingListDelegate::IsEnableAddLocation()
{
    if (m_simRingInfo.simRingLocationArry.size() >= MAX_LOCATION)
    {
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_MEMORY_FULL), XSI_MSGBOX_ID_NONE, 1500,
                       MESSAGEBOX_NOTE);
        return false;
    }

    return true;
}

int CSettinguiSimultaneousRingListDelegate::GetItemRealIndex(int nIndex)
{
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(nIndex);
        if (NULL != pItemData)
        {
            return atoi(pItemData->GetItemID().c_str());
        }
    }

    return -1;
}

#endif
