#include "settinguiexecutiveandassistantdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#include "keymap.h"
#include "common/common_data_define.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguimanager.h"
#include "editcheckmethod.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define MAX_ALTER_ROLLOVER_TIMER 9999
#define MIN_ALTER_ROLLOVER_TIMER 0
}


CSettinguiExecutiveAndAssistantDelegate::CSettinguiExecutiveAndAssistantDelegate()
    : m_eUIType(PAGE_UI_TYPE_NONE)
    , m_eUserType(EET_None)
    , m_bQuitPage(true)
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiExecutiveAndAssistantDelegate::OnMessage);
}

CSettinguiExecutiveAndAssistantDelegate::~CSettinguiExecutiveAndAssistantDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiExecutiveAndAssistantDelegate::OnMessage);
}

CSettingUIDelegateBase *
CSettinguiExecutiveAndAssistantDelegate::CreateExecutiveAndAssistantDelegate()
{
    CSettinguiExecutiveAndAssistantDelegate * pDelegate = new CSettinguiExecutiveAndAssistantDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiExecutiveAndAssistantDelegate::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiExecutiveAndAssistantDelegate * pTopDelegate = (CSettinguiExecutiveAndAssistantDelegate *)
            g_pSettingUIManager->GetTopDelegate();
    if (NULL == pTopDelegate)
    {
        return FALSE;
    }

    yl::string strPageID = pTopDelegate->GetPageID();

    if (kszPageIDCallControllExecutiveFilter != strPageID
            && kszPageIDCallControllExecutiveScreen != strPageID
            && kszPageIDCallControllExecutiveAlter != strPageID
            && kszPageIDCallControllExecutiveCommon != strPageID
            && kszPageIDCallControllExecutiveListview != strPageID
            && kszPageIDCallControllAssistantCommon != strPageID
            && kszPageIDCallControllAssistantListview != strPageID)
    {
        return FALSE;
    }

    switch (msg.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            pTopDelegate->OnMessageReqFinish(msg);
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            pTopDelegate->OnMessageReqFail(msg);
        }
        break;
    case XSI_MESSAGE_FINISH_ALL:
        {
            pTopDelegate->OnMessageFinishAll(msg);
        }
        break;
    default:
        {
            return FALSE;
        }
        break;
    }

    return TRUE;
}

void CSettinguiExecutiveAndAssistantDelegate::OnMessageReqFinish(msgObject & msg)
{
    bool bUpdatePage = false;
    // 取消提示框
    if (XSI_EXECUTIVE_ASSISTANTLIST_GET <= msg.wParam && XSI_ASSISTANT_EXECUTIVELIST_GET >= msg.wParam)
    {
        MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_LOADING);
        bUpdatePage = true;
    }
    else if (XSI_EXECUTIVE_ASSISTANTLIST_PUT <= msg.wParam
             && XSI_ASSISTANT_EXECUTIVELIST_PUT >= msg.wParam)
    {
        MessageBox_RemoveMessageBox(this, MessageBox_Close, XSI_MSGBOX_ID_SAVE);

        if (msg.wParam == XSI_ASSISTANT_EXECUTIVELIST_PUT && (PAGEUI_TYPE_LISTVIEW == m_eUIType
                || PAGE_UI_TYPE_DETAIL == m_eUIType))
        {
            // Push Executive List 页面比较特殊，因为分片的原因，不能退出页面，但是需要刷新数据
            bUpdatePage = true;
        }
    }

    // 是否需要刷新
    bool bQuitPage = !bUpdatePage;
    if (bUpdatePage)
    {
        bQuitPage = !UpdateUIData();
    }

    // 有刷新的情况下，不退出页面
    if (bQuitPage)
    {
        g_SetUICallBackFun.FallBackToPrevousPage();
    }
}

void CSettinguiExecutiveAndAssistantDelegate::OnMessageReqFail(msgObject & msg)
{
    m_bQuitPage = false;

    SXsiExtenInfo * pInfo = (SXsiExtenInfo *)msg.GetExtraData();
    if (pInfo != NULL)
    {
        if (PAGEUI_TYPE_LISTVIEW == m_eUIType && IsAssistant() && XSI_MSGBOX_ID_SAVE == m_eMsgboxID)
        {
            MessageBox_RemoveMessageBox(this, MessageBox_Close, m_eMsgboxID);

            if (XSI_MSGBOX_ID_LOADING == m_eMsgboxID)
            {
                g_SetUICallBackFun.RefreshUI(false);
            }

            // 18555 说明：Executive 禁用了 Alow opt in/out
            if (pInfo->m_nError == 400) // && pInfo->m_nEntenParm == 18555)
            {
                UpdateUIData(false);

                SetFocusIndex(pInfo->m_nEntenParm);

                m_eMsgboxID = XSI_MSGBOX_ID_FAIL;
                ShowMessageBox(_LANG(TRID_WARNING),  _LANG(TRID_NOT_ALLOW_TO_OPTIN_OUT), m_eMsgboxID, LONGER_TIME,
                               MESSAGEBOX_Cancel);

                return;
            }
        }
    }

    CSettinguiXSIDelegatebase::OnMessageReqFail(msg);
}

void CSettinguiExecutiveAndAssistantDelegate::OnMessageFinishAll(msgObject & msg)
{
    if (m_bQuitPage)
    {
        if (XSI_MSGBOX_ID_NONE != m_eMsgboxID)
        {
            MessageBox_RemoveMessageBox(this, MessageBox_Close, m_eMsgboxID);
        }

        g_SetUICallBackFun.FallBackToPrevousPage();
    }
}

void CSettinguiExecutiveAndAssistantDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nID  = pMessageBox->GetID();
    int nResult = pMessageBox->GetResult();

    if (XSI_MSGBOX_ID_FAIL == nID)
    {
        if (MessageBox_TimeOut == nResult || MessageBox_Cancel == nResult)
        {
            if (MessageBox_Cancel == nResult && PAGE_UI_TYPE_NONE != m_eUIType)
            {
                // 手动取消，退出当前页
                g_SetUICallBackFun.FallBackToPrevousPage();
            }
            else
            {
                g_SetUICallBackFun.RefreshUI(false, GetPageID());
            }
            return;
        }
    }
    else if (MessageBox_Cancel == nResult && (XSI_MSGBOX_ID_LOADING == nID
             || XSI_MSGBOX_ID_SAVE == nID))
    {
        if (PAGE_UI_TYPE_DETAIL != m_eUIType && PAGE_UI_TYPE_NONE != m_eUIType)
        {
            OnCancleClick();
            g_SetUICallBackFun.FallBackToPrevousPage();
            return;
        }
    }

    CSettinguiXSIDelegatebase::MessageBoxCallBack(pMessageBox);
}

void CSettinguiExecutiveAndAssistantDelegate::InitExtraData(void * pData)
{
    CSettinguiXSIDelegatebase::InitExtraData(pData);

    int iDefaultAccount = BWExeAssis_GetLineId();
    if (iDefaultAccount != m_iLineID)
    {
        BWExeAssis_SetLineId(m_iLineID);
    }

    InitPageTypeData();
}

CSettingUIPageDataBase * CSettinguiExecutiveAndAssistantDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData && IsHasAccountRegister())
    {
        if (PAGEUI_TYPE_ALERT == m_eUIType)
        {
            AddComboBoxOption(EXECUTIVE_ALTER_TIMER_TO_NEXT);
            AddComboBoxOption(EXECUTIVE_ALTER_TIMER_RECALL_AFTER);
        }

        ExeAssis_Action eAction = MapUIType2ActionType();

        if (m_eUIType != PAGE_UI_TYPE_DETAIL && m_eUIType != PAGE_UI_TYPE_NONE)
        {
            ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), XSI_MSGBOX_ID_LOADING, -1,
                           MESSAGEBOX_Cancel);

            m_eMsgboxID = XSI_MSGBOX_ID_LOADING;

            BWExeAssis_QueryAction(eAction, m_iLineID);
        }
    }

    return m_pPageData;
}

bool CSettinguiExecutiveAndAssistantDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettinguiXSIDelegatebase::PageDataFilter(pPagedata, bReload);

    if (PAGEUI_TYPE_FILTER == m_eUIType)
    {
        bool bShow = (EFM_Simple == GetComboboxValue(EXECUTIVE_FILTER_MODE)) ? true : false;
        SetItemShow(pPagedata, EXECUTIVE_FILTER_TYPE, bShow);
    }
    else if (PAGEUI_TYPE_COMMON == m_eUIType && IsAssistant())
    {
        bool bShow = (1 == GetComboboxValue(ASSISTANT_DIVERT_SWITCH)) ? true : false;
        SetItemShow(pPagedata, ASSISTANT_DIVERT_TO, bShow);
    }
    else if (PAGEUI_TYPE_ALERT == m_eUIType)
    {
        bool bShow = (ERA_FWD_SIPURI == GetComboboxValue(EXECUTIVE_ALTER_ROLLOVER_ACTION)) ? true : false;
        SetItemShow(pPagedata, EXECUTIVE_ALTER_ROLLOVER_FWD_TO, bShow);

        bool bEnable = (EAM_Sequential == GetComboboxValue(EXECUTIVE_ALTER_MODE)) ? true : false;
        SetItemEnable(pPagedata, EXECUTIVE_ALTER_TIMER_TO_NEXT, bEnable);

        bEnable = (ECIS_Custom == GetComboboxValue(EXECUTIVE_ALTER_CALLID_NAME)) ? true : false;
        SetItemEnable(pPagedata, EXECUTIVE_ALTER_CALLID_CUSTOM_NAME, bEnable);

        bEnable = (ECIS_Custom == GetComboboxValue(EXECUTIVE_ALTER_CALLID_NUMBER)) ? true : false;
        SetItemEnable(pPagedata, EXECUTIVE_ALTER_CALLID_CUSTOM_NUMBER, bEnable);
    }

    return bResult;
}

bool CSettinguiExecutiveAndAssistantDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !CheckValid() || !pPageData->IsEnable())
    {
        return false;
    }

    int iMsgBoxTimer = 10000;
    bool bHandled = false;
    switch (m_eUIType)
    {
    case PAGEUI_TYPE_FILTER:
        {
            CExeFilterInfo dataInfo;
            if (BWExeAssis_GetData(&dataInfo, m_iLineID))
            {
                dataInfo.bSwitchOn = (bool)GetComboboxValue(EXECUTIVE_FILTER_SWITCH);
                dataInfo.eFilterMode = (EXE_Filter_Mode)GetComboboxValue(EXECUTIVE_FILTER_MODE);
                dataInfo.eFilterType = (EXE_Filter_Type)GetComboboxValue(EXECUTIVE_FILTER_TYPE);

                bHandled = BWExeAssis_SetData(&dataInfo, m_iLineID);
            }
        }
        break;
    case PAGEUI_TYPE_SCREEN:
        {
            CExeScreenInfo dataInfo;

            dataInfo.bScreeningOn = (bool)GetComboboxValue(EXECUTIVE_SCREEN_SWITCH);
            dataInfo.eAlterType = (EXE_Screen_Alter_Type)GetComboboxValue(EXECUTIVE_SCREEN_TYPE);
            dataInfo.bAlertMobility = (bool)GetComboboxValue(EXECUTIVE_SCREEN_MOBILITY_SWITCH);
            dataInfo.bAlertAnywhere = (bool)GetComboboxValue(EXECUTIVE_SCREEN_ANYWHERE_SWITCH);
            dataInfo.bAlertSCA = (bool)GetComboboxValue(EXECUTIVE_SCREEN_SCA_SWITCH);

            bHandled = BWExeAssis_SetData(&dataInfo, m_iLineID);
        }
        break;
    case PAGEUI_TYPE_ALERT:
        {
            CExeAlterInfo dataInfo;

            dataInfo.eAlterMode = (EXE_Alter_Mode)GetComboboxValue(EXECUTIVE_ALTER_MODE);
            dataInfo.eCallInfoNameSource = (EXE_CallInfo_Source)GetComboboxValue(EXECUTIVE_ALTER_CALLID_NAME);
            dataInfo.eCallInfoNumberSource = (EXE_CallInfo_Source)GetComboboxValue(
                                                 EXECUTIVE_ALTER_CALLID_NUMBER);
            dataInfo.eRolloverAction = (EXE_Rollover_Action)GetComboboxValue(EXECUTIVE_ALTER_ROLLOVER_ACTION);

            dataInfo.iAdvanceToNextAssistantAfter = GetComboboxValue(EXECUTIVE_ALTER_TIMER_TO_NEXT) + 2;
            dataInfo.iCallPushRecallAfter = GetComboboxValue(EXECUTIVE_ALTER_TIMER_RECALL_AFTER) + 2;
            dataInfo.iRolloverAfter = GetComboboxValue(EXECUTIVE_ALTER_ROLLOVER_TIMER);

            dataInfo.strRolloverTo = GetValue(EXECUTIVE_ALTER_ROLLOVER_FWD_TO);
            dataInfo.strCustomCallName = GetValue(EXECUTIVE_ALTER_CALLID_CUSTOM_NAME);
            dataInfo.strCustomCallNumber = GetValue(EXECUTIVE_ALTER_CALLID_CUSTOM_NUMBER);

            CorrectData(dataInfo);
            bHandled = BWExeAssis_SetData(&dataInfo, m_iLineID);
        }
        break;
    case PAGEUI_TYPE_LISTVIEW:
        {
            if (IsAssistant())
            {
                CAssCommon dataInfo;
                if (BWExeAssis_GetData(&dataInfo, m_iLineID))
                {
                    int iListSize = dataInfo.listData.size();
                    for (int i = 0; i < iListSize; i++)
                    {
                        yl::string strItemID = commonAPI_FormatString("%s%d", EXECUTIVE_LIST_NO_DOT, i);
                        dataInfo.listData.at(i).bOptIn = (bool)GetComboboxValue(strItemID);
                    }

                    if (iListSize > 0)
                    {
                        bHandled = BWExeAssis_SetData(&dataInfo, m_iLineID, EAA_AssisCommon);
                    }
                    else
                    {
                        m_eMsgboxID = XSI_MSGBOX_ID_FAIL;
                        ShowMessageBox(_LANG(TRID_WARNING),  _LANG(TRID_NOTHING_CHANGED), m_eMsgboxID, LONGER_TIME,
                                       MESSAGEBOX_Cancel);
                        return true;
                    }
                }
            }
        }
        break;
    case PAGEUI_TYPE_COMMON:
        {
            if (IsExecutive())
            {
                CExeCommon dataInfo;
                if (BWExeAssis_GetData(&dataInfo, m_iLineID))
                {
                    dataInfo.bAllowOpt = (bool)GetComboboxValue(EXECUTIVE_DETAILS_ALLOW_OPT);
                    bHandled = BWExeAssis_SetData(&dataInfo, m_iLineID);
                }
            }
            else if (IsAssistant())
            {
                CAssCommon dataInfo;
                if (BWExeAssis_GetData(&dataInfo, m_iLineID))
                {
                    dataInfo.bDivertOn = (bool)GetComboboxValue(ASSISTANT_DIVERT_SWITCH);
                    dataInfo.strDivertNumber = GetValue(ASSISTANT_DIVERT_TO);

                    bHandled = BWExeAssis_SetData(&dataInfo, m_iLineID);
                }
            }
        }
        break;
    default:
        break;
    }

    if (!bHandled)
    {
        iMsgBoxTimer = LONGER_TIME;
    }

    m_bQuitPage = true;
    m_eMsgboxID = XSI_MSGBOX_ID_SAVE;
    ShowMessageBox(_LANG(TRID_NOTE),  _LANG(TRID_SAVING_PLEASE_WAIT), m_eMsgboxID, iMsgBoxTimer,
                   MESSAGEBOX_Cancel);

    return bHandled;
}

bool CSettinguiExecutiveAndAssistantDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if ((PHONE_KEY_HANDFREE == nKeyCode || PHONE_KEY_HANDSET == nKeyCode
            || PHONE_KEY_HANDSET_OFF_HOOK == nKeyCode)
            && PAGEUI_TYPE_LISTVIEW == m_eUIType)
    {
        return CallOut();
    }

    return CSettinguiXSIDelegatebase::OnKeyEvent(nKeyCode, bPress, nDataIndex);
}

bool CSettinguiExecutiveAndAssistantDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType && PAGEUI_TYPE_LISTVIEW == m_eUIType && IsExecutive())
    {
        return CallOut(iDataIndex);
    }

    return CSettinguiXSIDelegatebase::OnAction(eActionType, iDataIndex);
}

bool CSettinguiExecutiveAndAssistantDelegate::OnSoftkeyAction(int iType, bool bLongPress,
        int nDataIndex)
{
    if (PAGEUI_TYPE_LISTVIEW == m_eUIType)
    {
        if (ST_CALLINITIATION == iType && IsAssistant())
        {
            return OnCallInitiationPress();
        }
        else if (ST_SEND == iType)
        {
            return CallOut();
        }
    }

    return CSettinguiXSIDelegatebase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CSettinguiExecutiveAndAssistantDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData &
        vecSoftKey)
{
    if (SOFTKEY_NUM != vecSoftKey.size())
    {
        return;
    }

    if (PAGEUI_TYPE_LISTVIEW == m_eUIType)
    {
        vecSoftKey[0].m_iType = ST_BACK;
        vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);

        if (NULL == m_pPageData)
        {
            return;
        }
        int nItemCount = m_pPageData->GetTotalItemNumber();

        if (IsExecutive() && nItemCount > 0)
        {
            vecSoftKey[3].m_iType = ST_SEND;
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SEND);
        }
        else if (IsAssistant() && nItemCount > 0)
        {
            vecSoftKey[1].m_iType = ST_CALLINITIATION;
            vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_DSSKEY_CALL_INITIATION);
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
    }
}

void CSettinguiExecutiveAndAssistantDelegate::OnComboBoxValueChange(int nIndex)
{
    yl::string strItemID = GetItemIDByDataIndex(nIndex);
    if (EXECUTIVE_FILTER_MODE == strItemID
            || ASSISTANT_DIVERT_SWITCH == strItemID
            || EXECUTIVE_ALTER_ROLLOVER_ACTION == strItemID
            || EXECUTIVE_ALTER_MODE == strItemID
            || EXECUTIVE_ALTER_CALLID_NAME == strItemID
            || EXECUTIVE_ALTER_CALLID_NUMBER == strItemID)
    {
        g_SetUICallBackFun.RefreshUI(false, GetPageID());
    }
}

void CSettinguiExecutiveAndAssistantDelegate::OnCancleClick()
{
    BWExeAssis_CancelAllRequest();
}

bool CSettinguiExecutiveAndAssistantDelegate::IsResetFocusWhileReload()
{
    if (PAGEUI_TYPE_LISTVIEW == m_eUIType)
    {
        return true;
    }

    return false;
}

void CSettinguiExecutiveAndAssistantDelegate::OnEnterPage(const yl::string & strPage,
        int iDataIndex)
{
    PageOpenData opPageData(strPage);
    opPageData.pExtraData = (void *)&m_iLineID;
    g_SetUICallBackFun.EnterPage(opPageData);
}

void CSettinguiExecutiveAndAssistantDelegate::AddComboBoxOption(const yl::string & strItemID)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIComboBoxItemDataPtr pComboItem = CSettingUIComboBoxItemData::GetComboBoxDataItem(
                m_pPageData->GetItemByItemId(strItemID));
    if (NULL != pComboItem)
    {
        for (int i = 2; i <= 20; ++i)
        {
            CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;

            if (NULL != pOption)
            {
                pOption->m_strDisplayName = commonAPI_FormatString("%d %s", i, _LANG(TRID_RINGS).c_str());
                pOption->m_strItemVale = commonAPI_FormatString("%d", i - 2);

                if (!pComboItem->AddOptionItem(pOption))
                {
                    delete pOption;
                }
            }
        }
    }
}

bool CSettinguiExecutiveAndAssistantDelegate::UpdateUIData(bool bRefresh /*= true*/)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    bool bRet = false;

    switch (m_eUIType)
    {
    case PAGEUI_TYPE_FILTER:
        {
            CExeFilterInfo dataInfo;
            if (BWExeAssis_GetData(&dataInfo, m_iLineID))
            {
                SetComboboxValue(EXECUTIVE_FILTER_SWITCH, dataInfo.bSwitchOn);
                SetComboboxValue(EXECUTIVE_FILTER_MODE, (int)dataInfo.eFilterMode);
                SetComboboxValue(EXECUTIVE_FILTER_TYPE, (int)dataInfo.eFilterType);
                bRet = true;
            }
        }
        break;
    case PAGEUI_TYPE_SCREEN:
        {
            CExeScreenInfo dataInfo;
            if (BWExeAssis_GetData(&dataInfo, m_iLineID))
            {
                SetComboboxValue(EXECUTIVE_SCREEN_SWITCH, dataInfo.bScreeningOn);
                SetComboboxValue(EXECUTIVE_SCREEN_TYPE, (int)dataInfo.eAlterType);
                SetComboboxValue(EXECUTIVE_SCREEN_MOBILITY_SWITCH, dataInfo.bAlertMobility);
                SetComboboxValue(EXECUTIVE_SCREEN_ANYWHERE_SWITCH, dataInfo.bAlertAnywhere);
                SetComboboxValue(EXECUTIVE_SCREEN_SCA_SWITCH, dataInfo.bAlertSCA);
                bRet = true;
            }
        }
        break;
    case PAGEUI_TYPE_ALERT:
        {
            CExeAlterInfo dataInfo;
            if (BWExeAssis_GetData(&dataInfo, m_iLineID))
            {
                CorrectData(dataInfo);

                SetComboboxValue(EXECUTIVE_ALTER_MODE, (int)dataInfo.eAlterMode);
                SetComboboxValue(EXECUTIVE_ALTER_TIMER_TO_NEXT, dataInfo.iAdvanceToNextAssistantAfter - 2);
                SetComboboxValue(EXECUTIVE_ALTER_CALLID_NAME, (int)dataInfo.eCallInfoNameSource);
                SetValue(EXECUTIVE_ALTER_CALLID_CUSTOM_NAME, dataInfo.strCustomCallName);
                SetComboboxValue(EXECUTIVE_ALTER_CALLID_NUMBER, (int)dataInfo.eCallInfoNumberSource);
                SetValue(EXECUTIVE_ALTER_CALLID_CUSTOM_NUMBER, dataInfo.strCustomCallNumber);
                SetComboboxValue(EXECUTIVE_ALTER_TIMER_RECALL_AFTER, dataInfo.iCallPushRecallAfter - 2);
                SetComboboxValue(EXECUTIVE_ALTER_ROLLOVER_TIMER, dataInfo.iRolloverAfter);
                SetComboboxValue(EXECUTIVE_ALTER_ROLLOVER_ACTION, (int)dataInfo.eRolloverAction);
                SetValue(EXECUTIVE_ALTER_ROLLOVER_FWD_TO, dataInfo.strRolloverTo);
                bRet = true;
            }
        }
        break;
    case PAGEUI_TYPE_LISTVIEW:
        {
            int iIndex = 0;

            //清空所有item
            m_pPageData->ClearItemData();

            if (IsExecutive())
            {
                CExeCommon dataInfo;
                if (BWExeAssis_GetData(&dataInfo, m_iLineID))
                {
                    for (ExeAssisListIt it = dataInfo.listData.begin(); it != dataInfo.listData.end(); ++it)
                    {
                        yl::string strItemID = commonAPI_FormatString("%s%d", ASSISTANT_LIST_NO_DOT, iIndex);
                        yl::string strDisp = it->ItemName.strUserId;
                        bool bOptIn = it->bOptIn;
                        yl::string strValue = bOptIn ? _LANG(TRID_OPT_IN) : _LANG(TRID_OPT_OUT);

                        AddExecutiveListviewItem(strItemID, strDisp, strValue);
                        ++iIndex;
                    }

                    bRet = true;
                }
            }
            else if (IsAssistant())
            {
                CAssCommon dataInfo;
                if (BWExeAssis_GetData(&dataInfo, m_iLineID))
                {
                    for (ExeAssisListIt it = dataInfo.listData.begin(); it != dataInfo.listData.end(); ++it)
                    {
                        yl::string strItemID = commonAPI_FormatString("%s%d", EXECUTIVE_LIST_NO_DOT, iIndex);
                        yl::string strDisp = it->ItemName.strUserId;
                        yl::string strValue = it->bOptIn ? "1" : "0";

                        AddAssistantListviewItem(strItemID, strDisp, strValue);
                        ++iIndex;
                    }

                    bRet = true;
                }
            }
        }
        break;
    case PAGEUI_TYPE_COMMON:
        {
            if (IsExecutive())
            {
                CExeCommon dataInfo;
                if (BWExeAssis_GetData(&dataInfo, m_iLineID))
                {
                    SetComboboxValue(EXECUTIVE_DETAILS_ALLOW_OPT, dataInfo.bAllowOpt);
                    bRet = true;
                }
            }
            else if (IsAssistant())
            {
                CAssCommon dataInfo;
                if (BWExeAssis_GetData(&dataInfo, m_iLineID))
                {
                    SetComboboxValue(ASSISTANT_DIVERT_SWITCH, dataInfo.bDivertOn);
                    SetValue(ASSISTANT_DIVERT_TO, dataInfo.strDivertNumber);
                    bRet = true;
                }
            }
        }
        break;
    case PAGE_UI_TYPE_DETAIL:
        {
            bRet = true;
        }
        break;
    default:
        break;
    }

    if (bRet && bRefresh)
    {
        //更新界面
        g_SetUICallBackFun.RefreshUI();
    }

    return bRet;
}

bool CSettinguiExecutiveAndAssistantDelegate::IsHasAccountRegister()
{
    int iAvailAccount = BWExeAssis_GetAccountCountByType((EEA_TYPE)m_eUserType);
    if (iAvailAccount <= 0)
    {
        ShowMessageBox(_LANG(TRID_WARNING), _LANG(TRID_NO_ACCOUNT), XSI_MSGBOX_ID_NO_ACCOUNT, 1500,
                       MESSAGEBOX_NOTE);
        return false;
    }

    return true;
}

void CSettinguiExecutiveAndAssistantDelegate::AddExecutiveListviewItem(const yl::string & strItemID,
        const yl::string & strDisplayName, const yl::string & strValue)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    AddBaseItemData(m_pPageData, strDisplayName, strValue, strItemID);
}

void CSettinguiExecutiveAndAssistantDelegate::AddAssistantListviewItem(const yl::string & strItemID,
        const yl::string & strDisplayName, const yl::string & strValue)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIComboBoxItemDataPtr pComboItem = new CSettingUIComboBoxItemData;
    if (NULL != pComboItem)
    {
        pComboItem->SetItemID(strItemID);
        pComboItem->SetDisplayName(strDisplayName);
        pComboItem->SetValue(strValue);
        pComboItem->SetShowIndex(true);
        pComboItem->SetSwitchSoftKey(true);

        for (int i = 0; i <= 2; ++i)
        {
            CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;

            if (NULL != pOption)
            {
                pOption->m_strDisplayName = (0 == i) ? _LANG(TRID_OPT_OUT) : _LANG(TRID_OPT_IN);
                pOption->m_strItemVale = commonAPI_FormatString("%d", i);

                if (!pComboItem->AddOptionItem(pOption))
                {
                    delete pOption;
                }
            }
        }

        if (!m_pPageData->AddItem(pComboItem))
        {
            delete pComboItem;
        }
    }
}

void CSettinguiExecutiveAndAssistantDelegate::InitPageTypeData()
{
    m_strPageID;
    if (kszPageIDCallControllExecutiveDetails == m_strPageID)
    {
        m_eUIType = PAGE_UI_TYPE_DETAIL;
        m_eUserType = EET_Executive;
    }
    else if (kszPageIDCallControllExecutiveFilter == m_strPageID)
    {
        m_eUIType = PAGEUI_TYPE_FILTER;
        m_eUserType = EET_Executive;
    }
    else if (kszPageIDCallControllExecutiveScreen == m_strPageID)
    {
        m_eUIType = PAGEUI_TYPE_SCREEN;
        m_eUserType = EET_Executive;
    }
    else if (kszPageIDCallControllExecutiveAlter == m_strPageID)
    {
        m_eUIType = PAGEUI_TYPE_ALERT;
        m_eUserType = EET_Executive;
    }
    else if (kszPageIDCallControllExecutiveCommon == m_strPageID)
    {
        m_eUIType = PAGEUI_TYPE_COMMON;
        m_eUserType = EET_Executive;
    }
    else if (kszPageIDCallControllExecutiveListview == m_strPageID)
    {
        m_eUIType = PAGEUI_TYPE_LISTVIEW;
        m_eUserType = EET_Executive;
    }
    else if (kszPageIDCallControllAssistantDetails == m_strPageID)
    {
        m_eUIType = PAGE_UI_TYPE_DETAIL;
        m_eUserType = EET_Assistant;
    }
    else if (kszPageIDCallControllAssistantCommon == m_strPageID)
    {
        m_eUIType = PAGEUI_TYPE_COMMON;
        m_eUserType = EET_Assistant;
    }
    else if (kszPageIDCallControllAssistantListview == m_strPageID)
    {
        m_eUIType = PAGEUI_TYPE_LISTVIEW;
        m_eUserType = EET_Assistant;
    }
    else
    {
        m_eUIType = PAGE_UI_TYPE_NONE;
        m_eUserType = EET_None;
    }
}

bool CSettinguiExecutiveAndAssistantDelegate::CheckValid()
{
    if (PAGEUI_TYPE_ALERT == m_eUIType)
    {
        if (!CheckEmpty(EXECUTIVE_ALTER_CALLID_CUSTOM_NAME, EXECUTIVE_ALTER_CALLID_NAME, ECIS_Custom)
                || !CheckEmpty(EXECUTIVE_ALTER_CALLID_CUSTOM_NUMBER, EXECUTIVE_ALTER_CALLID_NUMBER, ECIS_Custom)
                || !CheckEmpty(EXECUTIVE_ALTER_ROLLOVER_FWD_TO, EXECUTIVE_ALTER_ROLLOVER_ACTION, ERA_FWD_SIPURI))
        {
            return false;
        }
        else if (!CheckDigit(EXECUTIVE_ALTER_ROLLOVER_TIMER, MIN_ALTER_ROLLOVER_TIMER,
                             MAX_ALTER_ROLLOVER_TIMER))
        {
            return false;
        }
    }
    else if (PAGEUI_TYPE_COMMON == m_eUIType)
    {
        if (IsAssistant())
        {
            if (!CheckEmpty(ASSISTANT_DIVERT_TO, ASSISTANT_DIVERT_SWITCH, TRUE))
            {
                return false;
            }
        }
    }

    return true;
}


bool CSettinguiExecutiveAndAssistantDelegate::CheckDigit(const yl::string & strItemName,
        int minValue, int maxValue)
{
    if (NULL == m_pPageData || strItemName.empty())
    {
        return false;
    }

    CSettingUIItemDataBasePtr pItem = m_pPageData->GetItemByItemId(strItemName);
    if (NULL == pItem || !pItem->IsShow() || pItem->IsEnable())
    {
        return true;
    }

    yl::string strValue = pItem->GetValue();
    if (!strValue.empty())
    {
        if (IsValidNumber(strValue.c_str()))
        {
            int iValue = atoi(strValue.c_str());

            if (iValue >= minValue && iValue <= maxValue)
            {
                return true;
            }
        }
    }

    int nIndex = GetItemDataIndexByItemId(strItemName);
    SetFocusIndex(nIndex);

    m_eMsgboxID = XSI_MSGBOX_ID_FAIL;
    ShowMessageBox(_LANG(TRID_WARNING), _LANG(TRID_INVALID_RANGE_VALUE), m_eMsgboxID, LONGER_TIME,
                   MESSAGEBOX_Cancel);

    return false;
}

bool CSettinguiExecutiveAndAssistantDelegate::CheckEmpty(const yl::string & strItemName,
        const yl::string & strItemDependName /*= ""*/, int iDependValue /*= 0*/)
{
    if (NULL == m_pPageData || strItemName.empty())
    {
        return false;
    }

    // 是否可见，如果不可见，不需要check
    CSettingUIItemDataBasePtr pItem = m_pPageData->GetItemByItemId(strItemName);
    if (NULL == pItem || !pItem->IsShow() || !pItem->IsEnable())
    {
        return true;
    }

    if (!strItemDependName.empty())
    {
        // 依赖的项不满足条件，不需要check
        CSettingUIItemDataBasePtr pDependItem = m_pPageData->GetItemByItemId(strItemDependName);
        if (NULL == pDependItem || !pDependItem->IsShow() || !pDependItem->IsEnable())
        {
            return true;
        }

        if (atoi(pDependItem->GetValue().c_str()) != iDependValue)
        {
            return true;
        }
    }

    if (!pItem->GetValue().empty())
    {
        return true;
    }

    int nIndex = GetItemDataIndexByItemId(strItemName);
    SetFocusIndex(nIndex);

    m_eMsgboxID = XSI_MSGBOX_ID_FAIL;
    ShowMessageBox(_LANG(TRID_WARNING), _LANG(TRID_TARGET_CANNOT_BE_EMPTY), m_eMsgboxID, LONGER_TIME,
                   MESSAGEBOX_Cancel);

    return false;
}

bool CSettinguiExecutiveAndAssistantDelegate::CallOut(int nFocusIndex /*= -1*/)
{
    nFocusIndex = (-1 == nFocusIndex) ? GetFocusIndex() : nFocusIndex;

    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pFocusItem = GetItemDataByDataIndex(nFocusIndex);
        if (NULL != pFocusItem)
        {
            return talklogic_CallOut(pFocusItem->GetDisplayName(), "", m_iLineID);
        }
    }

    return false;
}

bool CSettinguiExecutiveAndAssistantDelegate::OnCallInitiationPress()
{
    if (NULL != m_pPageData && PAGEUI_TYPE_LISTVIEW == m_eUIType && IsAssistant())
    {
        CSettingUIItemDataBasePtr pFocusItem = GetItemDataByDataIndex(GetFocusIndex());
        if (NULL != pFocusItem)
        {
            if (BWExeAssis_SetCallInitiationNumber(pFocusItem->GetDisplayName(), m_iLineID))
            {
                return talklogic_EnterDialUI("", m_iLineID, true, TCT_CALL_INITIATION);
            }
        }
    }

    return false;
}

void CSettinguiExecutiveAndAssistantDelegate::CorrectData(CExeAlterInfo & dataInfo)
{
    if (dataInfo.iCallPushRecallAfter < 2 || dataInfo.iCallPushRecallAfter > 20)
    {
        dataInfo.iCallPushRecallAfter = 2;
    }

    if (dataInfo.iAdvanceToNextAssistantAfter < 2 || dataInfo.iAdvanceToNextAssistantAfter > 20)
    {
        dataInfo.iAdvanceToNextAssistantAfter = 2;
    }
}

ExeAssis_Action CSettinguiExecutiveAndAssistantDelegate::MapUIType2ActionType()
{
    switch (m_eUIType)
    {
    case PAGEUI_TYPE_FILTER:
        return EAA_Filter;
    case PAGEUI_TYPE_SCREEN:
        return EAA_Screen;
    case PAGEUI_TYPE_ALERT:
        return EAA_Alter;
    case PAGEUI_TYPE_LISTVIEW:
    case PAGEUI_TYPE_COMMON:
        {
            if (IsExecutive())
            {
                return EAA_ExeCommon;
            }
            else if (IsAssistant())
            {
                return EAA_AssisCommon;
            }
        }
        break;
    case PAGE_UI_TYPE_DETAIL:
    default:
        break;
    }

    return EAA_None;
}

#endif
