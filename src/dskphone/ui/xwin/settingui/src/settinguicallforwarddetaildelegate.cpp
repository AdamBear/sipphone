#include "settinguicallforwarddetaildelegate.h"
#include <settinguilogic/include/settingdefine.h>
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "editcheckmethod.h"
#include "baseui/include/baseuicommon.h"
#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/dataloader.h"
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibaseaccountpagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/callforwardcontroller.h"
#include "settinguilogic/include/common.h"
#include "common/common_data_define.h"
#include "account/include/modaccount.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "netmsgtypedefine.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#if IF_SUPPORT_SELECT_CONTACT_INFO
#include "settinguimanager.h"
#include "settingui/include/modsettingui.h"
#endif

namespace
{
#define MSGBOX_ID_TARGER_ERROR 0
#define INDEX_TARGET 1
#define INDEX_OFF_CODE 3
#define MSGBOX_ID_ALL_LINE 22
}

CSettingUICallForwardDetailDelegate::CSettingUICallForwardDetailDelegate()
    : m_nForwardType(-1)
{
}

CSettingUICallForwardDetailDelegate::~CSettingUICallForwardDetailDelegate()
{
}

CSettingUIDelegateBase * CSettingUICallForwardDetailDelegate::CreateCallForwardDetailDelegate()
{
    CSettingUICallForwardDetailDelegate * pCallForwardDelegate = new
    CSettingUICallForwardDetailDelegate;

    return (CSettingUIDelegateBasePtr)pCallForwardDelegate;
}

#if IF_SUPPORT_SELECT_CONTACT_INFO
void CSettingUICallForwardDetailDelegate::UpdateContactInfo(const yl::string& strContactName, const yl::string& strContactNumber)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDAlwaysForward);
    if (NULL == pDelegate)
    {
        pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDBusyForward);
        if (NULL == pDelegate)
        {
            pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDNoAnswerForward);
        }
    }

    if (NULL == pDelegate)
    {
        return;
    }

    CSettingUICallForwardDetailDelegate* pCallforwardDelegate = static_cast<CSettingUICallForwardDetailDelegate*>(pDelegate);
    if (NULL == pCallforwardDelegate)
    {
        return;
    }

    pCallforwardDelegate->UpdateCallforwardData(strContactNumber);
}

void CSettingUICallForwardDetailDelegate::UpdateCallforwardData(const yl::string& strContactNumber)
{
    SetValue(CALLFORWARD_TARGET, strContactNumber);
}

bool CSettingUICallForwardDetailDelegate::IsShowDirectorySoftkey()
{
    yl::string strItemID = GetItemIDByDataIndex(GetFocusIndex());
    yl::string strValue = GetValue(strItemID);
    if (CALLFORWARD_TARGET == strItemID && strValue.empty())
    {
        return true;
    }

    return false;
}
#endif

bool CSettingUICallForwardDetailDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /* = NULL */)
{
    if (SETTING_MessageBox_OK == nResult && MSGBOX_ID_ALL_LINE == nID && NULL != m_pPageData)
    {
        //获取当前数据
        ForwardData forwardData;
        forwardData.m_isOn = GetComboboxValue(CALLFORWARD_ENABLE);
        forwardData.m_strTarget = GetValue(CALLFORWARD_TARGET);
        forwardData.m_strOnCode = GetValue(CALLFORWARD_ONCODE);
        forwardData.m_strOffCode = GetValue(CALLFORWARD_OFCODE);

        CSettingUIDataValidInfo validInfo;
        if (forwardData.m_isOn && IsContentEmpty(forwardData.m_strTarget))
        {
            validInfo.SetValidIndex(INDEX_TARGET);
            validInfo.SetValidNote(_LANG(TRID_TARGET_CANNOT_BE_EMPTY));
            g_SetUICallBackFun.DealWithValidData(validInfo);
            return false;
        }

        //拷贝到所有账号
        int iAccountNum = acc_AccountNum();
        for (int i = 0; i < iAccountNum; i++)
        {
            if (acc_IsAccountAvailable(i))
            {
                g_pCallForwardController->SetForwardDataByType((FWD_TYPE)m_nForwardType, forwardData, i);
            }
        }

        ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE_AND_REFRESH);

        return true;
    }

    return CSettingUIAccountDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

CSettingUIPageDataBase * CSettingUICallForwardDetailDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        m_nForwardType = GetTypeById(m_pPageData);

        LoadPrivateData(m_pPageData);

        for (int i = 0; i < m_pPageData->GetTotalItemNumber(); ++i)
        {
            CSettingUIComboBoxItemDataPtr pItemData = CSettingUIComboBoxItemData::GetComboBoxDataItem(
                        m_pPageData->GetItemByDataIndex(i));
            if (NULL != pItemData)
            {
                bool bSwitch = (-1 != m_iLineID);
                pItemData->SetSwitchSoftKey(bSwitch);
            }

        }
    }

    return m_pPageData;
}

bool CSettingUICallForwardDetailDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIAccountDelegateBase::PageDataFilter(pPagedata, bReload);

    if (0 != configParser_GetConfigInt(kszBroadsoftActive))
    {
        SetItemShow(pPagedata, CALLFORWARD_ONCODE, false);
        SetItemShow(pPagedata, CALLFORWARD_OFCODE, false);
    }
    else
    {
        SetItemShow(pPagedata, CALLFORWARD_ONCODE, true);
        SetItemShow(pPagedata, CALLFORWARD_OFCODE, true);
    }

    return bResult;
}

bool CSettingUICallForwardDetailDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    if (NULL == m_pPageData || !pPageData->IsEnable())
    {
        return true;
    }

    ForwardData forwardData;
    forwardData.m_isOn = GetComboboxValue(CALLFORWARD_ENABLE);
    forwardData.m_strTarget = GetValue(CALLFORWARD_TARGET);
    forwardData.m_strOnCode = GetValue(CALLFORWARD_ONCODE);
    forwardData.m_strOffCode = GetValue(CALLFORWARD_OFCODE);

    if (FWD_TYPE_NO_ANSWER == m_nForwardType)
    {
        forwardData.m_nRingTimes = GetComboboxValue(CALLFORWARD_RINGTIME);
    }


    //如果开关为开且Target为空，弹窗提示
    CSettingUIDataValidInfo validInfo;
    if (forwardData.m_isOn && IsContentEmpty(forwardData.m_strTarget))
    {
        validInfo.SetValidIndex(INDEX_TARGET);
        validInfo.SetValidNote(_LANG(TRID_FORWARD_TRAGET_IS_INAVLID));
        g_SetUICallBackFun.DealWithValidData(validInfo);
        return false;
    }

    if (!IsContentEmpty(forwardData.m_strOnCode) && IsContentEmpty(forwardData.m_strOffCode))
    {
        validInfo.SetValidIndex(INDEX_OFF_CODE);
        validInfo.SetValidNote(_LANG(TRID_OFF_CODE_CANNOT_BE_EMPTY));
        g_SetUICallBackFun.DealWithValidData(validInfo);
        return false;
    }

    g_pCallForwardController->SetForwardDataByType((FWD_TYPE)m_nForwardType, forwardData, m_iLineID);

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE_AND_REFRESH);

    return true;
}

bool CSettingUICallForwardDetailDelegate::OnSoftkeyAction(int iType, bool bLongPress,
        int nDataIndex)
{
    if (ST_ALL_LINES == iType)
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, "",
                                               _LANG(TRID_IF_COPY_CONFIGURATION_TO_ALL_LINES)
                                               , SETTING_MESSAGEBOX_OkCancel, "", "", -1, MSGBOX_ID_ALL_LINE);
        return true;
    }
#if IF_SUPPORT_SELECT_CONTACT_INFO
    else if (ST_DIRECTORY == iType)
    {
        SettingUI_EnterPage(kszPageIDDirectory, (void*)&CSettingUICallForwardDetailDelegate::UpdateContactInfo);
    }
#endif

    return CSettingUIAccountDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}


void CSettingUICallForwardDetailDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
#if IF_SUPPORT_SELECT_CONTACT_INFO
    CDirectoryAssociateBaseDelegate::GetPagePrivateSoftkey(vecSoftKey);
#endif

    if (SOFTKEY_NUM != vecSoftKey.size())
    {
        return;
    }

    if (-1 != m_iLineID)
    {
        vecSoftKey[1].m_iType = ST_ALL_LINES;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ALL_LINES);
    }
    else
    {

        vecSoftKey[1].m_iType = ST_EMPTY;
        vecSoftKey[1].m_strSoftkeyTitle = "";
    }
}

int CSettingUICallForwardDetailDelegate::GetTypeById(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return FWD_TYPE_MANUAL;
    }

    int nForwardType = FWD_TYPE_MANUAL;
    yl::string strId = pPageData->GetPageID();
    if (kszPageIDAlwaysForward == strId)
    {
        nForwardType = FWD_TYPE_ALWAYS;
    }
    else if (kszPageIDBusyForward == strId)
    {
        nForwardType = FWD_TYPE_BUSY;
    }
    else if (kszPageIDNoAnswerForward == strId)
    {
        nForwardType = FWD_TYPE_NO_ANSWER;
    }
    else
    {
        nForwardType = FWD_TYPE_MANUAL;
    }

    return nForwardType;
    return 0;
}

void CSettingUICallForwardDetailDelegate::LoadPrivateData(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    ForwardData forwardData;
    if (g_pCallForwardController->GetForwardDataByType(&forwardData, (FWD_TYPE)m_nForwardType,
            m_iLineID))
    {
        SetComboboxValue(CALLFORWARD_ENABLE, forwardData.m_isOn);
        SetValue(CALLFORWARD_TARGET, forwardData.m_strTarget);
        SetValue(CALLFORWARD_ONCODE, forwardData.m_strOnCode);
        SetValue(CALLFORWARD_OFCODE, forwardData.m_strOffCode);

        if (FWD_TYPE_NO_ANSWER == m_nForwardType)
        {
            SetComboboxValue(CALLFORWARD_RINGTIME, forwardData.m_nRingTimes);
        }

    }
}
