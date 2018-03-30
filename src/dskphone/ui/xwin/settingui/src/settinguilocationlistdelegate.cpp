#include "settinguilocationlistdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_ANYWHERE

#include "settinguimanager.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "common/common_data_define.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "feature/include/modfeature.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/anywhere/include/anywherecontroller.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "messagebox/selectmessagebox.h"
#include "baseui/include/modbaseui.h"

CSettinguiLocationListDelegate::CSettinguiLocationListDelegate()
    : m_bLocationListCanEidit(true)
{
    etl_RegisterMsgHandle(XSI_MESSAGE_REQ_FINISH, XSI_MESSAGE_REQ_FINISH
                          , &CSettinguiLocationListDelegate::OnMessage);
    etl_RegisterMsgHandle(XSI_MESSAGE_REQ_FAIL, XSI_MESSAGE_REQ_FAIL
                          , &CSettinguiLocationListDelegate::OnMessage);
}

CSettinguiLocationListDelegate::~CSettinguiLocationListDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_REQ_FINISH, XSI_MESSAGE_REQ_FINISH
                            , &CSettinguiLocationListDelegate::OnMessage);
    etl_UnregisterMsgHandle(XSI_MESSAGE_REQ_FAIL, XSI_MESSAGE_REQ_FAIL
                            , &CSettinguiLocationListDelegate::OnMessage);
}

CSettingUIDelegateBase * CSettinguiLocationListDelegate::CreateLocationListDelegate()
{
    CSettinguiLocationListDelegate * pDelegate = new CSettinguiLocationListDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

// 对外提供的消息（XSI_MESSAGE_REQ_FAIL和XSI_MESSAGE_REQ_FINISH）处理函数
LRESULT CSettinguiLocationListDelegate::OnMessage(msgObject & msg)
{
    CSettinguiLocationListDelegate * pDelegate = (CSettinguiLocationListDelegate *)
            g_pSettingUIManager->GetPageDelegate(kszPageIDAnyWhereLocationList);
    if (NULL == pDelegate || pDelegate != g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (pDelegate->GetPageLineID() != msg.lParam)
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

void CSettinguiLocationListDelegate::OnMessageReqFinish(msgObject & msg)
{
    switch (msg.wParam)
    {
    // 下载Location List结束后
    case XSI_ANYWHERE_GET_LOCATION_LIST:
        {
            OnReturnLocationList(msg);
        }
        break;
    case XSI_ANYWHERE_DELETE_LOCATION:
        {
            OnDeleteFinish(msg);
        }
        break;
    default:
        break;
    }
}

void CSettinguiLocationListDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
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
                OnEnterPage(kszPageIDAnyWhereEditLocation, GetItemRealIndex(nDataIndex));
            }
            break;
        case OPTION_DELETE:
            {
                ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_IF_DELETE_LOCATION), XSI_MSGBOX_ID_DELETE, -1,
                               MESSAGEBOX_OkCancel);
            }
            break;
        case OPTION_DELETE_ALL:
            {
                ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_IF_DELETE_ALL_LOCATIONS), XSI_MSGBOX_ID_DELETE_ALL, -1,
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

    CSettinguiXSIDelegatebase::MessageBoxCallBack(pMessageBox);
}

void CSettinguiLocationListDelegate::InitExtraData(void * pData)
{
    CSettingUIAccountDelegateBase::InitExtraData(pData);

    m_bLocationListCanEidit = !feature_UserModeConfigItemReadOnly(kszPageIDAnyWhereLocationList);

    AnyWhere_SetLineID(m_iLineID);
}

CSettingUIPageDataBase * CSettinguiLocationListDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && IsHasAccountRegister() && NULL != m_pPageData)
    {
        m_eMsgboxID = XSI_MSGBOX_ID_LOADING;
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), m_eMsgboxID, -1,
                       MESSAGEBOX_Cancel);

        AnyWhere_RequestAllLocationList();
    }

    return m_pPageData;
}

bool CSettinguiLocationListDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || m_LocationArry.size() < 1 || !pPageData->IsEnable())
    {
        return false;
    }

    //获取数据
    CStringVector numberVector;
    for (int i = 0; i < m_pPageData->GetTotalItemNumber() && i < m_LocationArry.size(); ++i)
    {
        CSettingUIItemDataBasePtr pItem = m_pPageData->GetItemByDataIndex(i);
        //获取状态
        m_LocationArry.m_locatArr.at(i).m_bActive = ("0" == pItem->GetValue()) ? false : true;
        numberVector.push_back(m_LocationArry.m_locatArr.at(i).m_strPhoneNum);
    }

    //执行更改操作
    bool bExecModify = AnyWhere_RequestModifyMutilLocations(numberVector, m_LocationArry);

    //是否有执行修改动作，如果没有则提示一秒提示框
    int nTime = 0;
    if (!bExecModify)
    {
        nTime = 1500;
    }

    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), XSI_MSGBOX_ID_SAVE, nTime,
                   MESSAGEBOX_Cancel);

    m_eMsgboxID = XSI_MSGBOX_ID_SAVE;

    // http://10.3.5.199/Bug.php?BugID=64594
    etlSleep(100);

    return true;
}

void CSettinguiLocationListDelegate::LoadLocationListItem()
{
    if (NULL == m_pPageData)
    {
        return ;
    }

    m_pPageData->ClearItemData();

    for (int i = 0; i < (int)m_LocationArry.size(); ++i)
    {
        yl::string strItemID;
        strItemID = commonAPI_FormatString("%d", i);
        //如果名字为空，则显示号码
        yl::string strDisp = m_LocationArry.m_locatArr[i].m_strDescription.c_str();
        if (strDisp.empty())
        {
            strDisp = m_LocationArry.m_locatArr[i].m_strPhoneNum.c_str();
        }

        CSettingUIComboBoxItemDataPtr pItemData = new CSettingUIComboBoxItemData;
        if (NULL != pItemData)
        {
            pItemData->SetItemID(strItemID);
            pItemData->SetDisplayName(strDisp);
            pItemData->SetShowIndex(true);
            pItemData->SetShowColon(false);
            pItemData->SetIsSetupSoftkeyByItem(false);
            yl::string strValue = m_LocationArry.m_locatArr[i].m_bActive ? "1" : "0";
            pItemData->SetValue(strValue);

            for (int i = 0; i < 2; ++i)
            {
                CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;
                if (NULL == pOption)
                {
                    continue;
                }

                pOption->m_strDisplayName = (0 == i) ? _LANG(TRID_OFF) : _LANG(TRID_ON);
                pOption->m_strItemVale = commonAPI_FormatString("%d", i);

                if (!pItemData->AddOptionItem(pOption))
                {
                    delete pOption;
                }
            }

            if (!m_pPageData->AddItem(pItemData))
            {
                delete pItemData;
            }
        }
    }
}

void CSettinguiLocationListDelegate::OnEnterPage(const yl::string & strPage, int iDataIndex)
{
    PageOpenData opPageData(strPage);
    opPageData.pExtraData = (void *)&iDataIndex;
    g_SetUICallBackFun.EnterPage(opPageData);

    SetRefreshDataWhenTopDelegate(true);
}

bool CSettinguiLocationListDelegate::OnPageAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        //不可编辑时进入edit界面，可编辑时且为有Save时为保存
        if (!m_bLocationListCanEidit && m_LocationArry.size() > 0)
        {
            OnEnterPage(kszPageIDAnyWhereEditLocation, GetItemRealIndex(iDataIndex));
        }
        else if (m_LocationArry.size() > 0 && m_bLocationListCanEidit)
        {
            SavePageData(m_pPageData);
        }

        return true;
    }

    return CSettinguiXSIDelegatebase::OnPageAction(eActionType, iDataIndex);
}

bool CSettinguiLocationListDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
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
    else if (ST_ENTER == iType && !m_bLocationListCanEidit && m_LocationArry.size() > 0)
    {
        //进入edit lovation界面
        OnEnterPage(kszPageIDAnyWhereEditLocation, GetItemRealIndex(nDataIndex));
    }
    else if (ST_ADD == iType)
    {
        //进入Add lovation界面
        if (IsEnableAddLocation())
        {
            OnEnterPage(kszPageIDAnyWhereAddLocation, nDataIndex);
        }
    }
    else
    {
        return CSettingUIAccountDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
    }

    return true;
}

void CSettinguiLocationListDelegate::OnCancleClick()
{
    AnyWhere_CancelAllRequest();
}

void CSettinguiLocationListDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (SOFTKEY_NUM != vecSoftKey.size())
    {
        return;
    }


    if (m_LocationArry.size() > 0)
    {
        vecSoftKey[0].m_iType = ST_BACK;
        vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);

        if (m_bLocationListCanEidit)
        {
            vecSoftKey[1].m_iType = ST_ADD;
            vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ADD);
            vecSoftKey[2].m_iType = ST_OPTION;
            vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_OPTION);
            vecSoftKey[3].m_iType = ST_SAVE;
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SAVE);
        }
        else
        {
            vecSoftKey[1].m_iType = ST_EMPTY;
            vecSoftKey[1].m_strSoftkeyTitle = "";
            vecSoftKey[2].m_iType = ST_EMPTY;
            vecSoftKey[2].m_strSoftkeyTitle = "";
            vecSoftKey[3].m_iType = ST_ENTER;
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        }
    }
    else
    {
        vecSoftKey[0].m_iType = ST_BACK;
        vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);

        if (m_bLocationListCanEidit)
        {
            vecSoftKey[1].m_iType = ST_ADD;
            vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ADD);
        }
        else
        {
            vecSoftKey[1].m_iType = ST_EMPTY;
            vecSoftKey[1].m_strSoftkeyTitle = "";
        }
        vecSoftKey[2].m_iType = ST_EMPTY;
        vecSoftKey[2].m_strSoftkeyTitle = "";
        vecSoftKey[3].m_iType = ST_EMPTY;
        vecSoftKey[3].m_strSoftkeyTitle = "";
    }
}

void CSettinguiLocationListDelegate::OnLocationDelete(bool bDeleteAll)
{
    bool bDeleted = false;
    if (!bDeleteAll)
    {
        //删除单个Location
        int nIndex = GetFocusIndex();
        if (nIndex >= 0 && nIndex < (int)m_LocationArry.size())
        {
            bDeleted = true;
            AnyWhere_RequestDeleteLocation(m_LocationArry.m_locatArr.at(nIndex).m_strPhoneNum);
        }
    }
    else
    {
        bDeleted = true;
        //删除全部Location
        AnyWhere_RequestDeleteAllLocation();
    }

    if (bDeleted)
    {
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_DELETING_PLEASE_WAIT), XSI_MSGBOX_ID_DELETEING, -1,
                       MESSAGEBOX_Cancel);

        m_eMsgboxID = XSI_MSGBOX_ID_DELETEING;
    }
}

void CSettinguiLocationListDelegate::OnDeleteFinish(msgObject & msg)
{
    MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_DELETEING);
    g_SetUICallBackFun.RefreshUI(true, GetPageID());
}

// 加载Location List成功后，获取List
void CSettinguiLocationListDelegate::OnReturnLocationList(msgObject & msg)
{
    SETTINGUI_INFO("CAnywherePageUI::OnReturnLocationList(msgObject& msg)");
    // 取消提示框
    MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_LOADING);

    //获取Location数据
    m_LocationArry.clear();
    AnyWhere_GetAllLocationsQueryResultList(m_LocationArry);

    LoadLocationListItem();

    g_SetUICallBackFun.RefreshUI(false, GetPageID());
}

//返回是否可以增加新的Location
bool CSettinguiLocationListDelegate::IsEnableAddLocation() const
{
    int nLocationCount = AnyWhere_GetCachedLocationCount();
    return nLocationCount < 100;
}

int CSettinguiLocationListDelegate::GetItemRealIndex(int nIndex)
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
