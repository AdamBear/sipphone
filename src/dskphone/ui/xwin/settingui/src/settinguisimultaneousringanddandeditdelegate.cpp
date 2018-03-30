#include "settinguisimultaneousringanddandeditdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_PRIORITY_ALERT

#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define INDEX_PHONE_NUMBER 0
}

CSettinguiSimultaneousRingAddAndEditDeletae::CSettinguiSimultaneousRingAddAndEditDeletae()
    : m_bActiveWhenAdd(false)
    , m_bInAdd(false)
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiSimultaneousRingAddAndEditDeletae::OnMessage);
}

CSettinguiSimultaneousRingAddAndEditDeletae::~CSettinguiSimultaneousRingAddAndEditDeletae()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiSimultaneousRingAddAndEditDeletae::OnMessage);
}

CSettingUIDelegateBase *
CSettinguiSimultaneousRingAddAndEditDeletae::CreateSimultaneousRingAddAndEditDelegate()
{
    CSettinguiSimultaneousRingAddAndEditDeletae * pDelegate = new
    CSettinguiSimultaneousRingAddAndEditDeletae;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiSimultaneousRingAddAndEditDeletae::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(
            kszPageIDCallControllSimultaneousRingAdd);
    if (NULL == pDelegate)
    {
        pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDCallControllSimultaneousRingEdit);
    }
    CSettinguiSimultaneousRingAddAndEditDeletae * pAddAndEditDelegate =
        (CSettinguiSimultaneousRingAddAndEditDeletae *)pDelegate;
    if (NULL == pAddAndEditDelegate || pAddAndEditDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message && XSI_SIMULTANEOUSRING_PUT == msg.wParam)
    {
        MessageBox_RemoveMessageBox(pAddAndEditDelegate, MessageBox_OK, XSI_MSGBOX_ID_SAVE);
    }
    else if (XSI_MESSAGE_REQ_FAIL == msg.message)
    {
        pAddAndEditDelegate->OnMessageReqFail(msg);
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

void CSettinguiSimultaneousRingAddAndEditDeletae::InitExtraData(void * pData)
{
    m_bInAdd = (kszPageIDCallControllSimultaneousRingAdd == GetPageID()) ? true : false;

    if (m_bInAdd && NULL != pData)
    {
        int * iAccountID = (int *)pData;
        if (NULL != iAccountID)
        {
            m_bActiveWhenAdd = (1 == *iAccountID) ? true : false;
        }
    }
    else
    {
        CSettinguiXSIDelegatebase::InitExtraData(pData);
    }
}

CSettingUIPageDataBase * CSettinguiSimultaneousRingAddAndEditDeletae::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        SRP_GetSimultaneousRingInfo(m_simRingInfo);
        if (!m_bInAdd)
        {
            if (m_iLineID >= 0 && m_iLineID < m_simRingInfo.simRingLocationArry.size())
            {
                SetValue(SIMULTANEOUSRING_NUMBER, m_simRingInfo.simRingLocationArry[m_iLineID].strAddress);
                SetComboboxValue(SIMULTANEOUSRING_ANSWERCONFIRM,
                                 m_simRingInfo.simRingLocationArry[m_iLineID].bAnswerConfirmationRequired);
            }
        }
        else
        {
            SetComboboxValue(SIMULTANEOUSRING_ANSWERCONFIRM, 0);
        }
    }

    return m_pPageData;
}

bool CSettinguiSimultaneousRingAddAndEditDeletae::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    yl::string strPhoneNumber = GetValue(SIMULTANEOUSRING_NUMBER);
    bool bAnswerConfirm = ("0" == GetValue(SIMULTANEOUSRING_ANSWERCONFIRM)) ? false : true;
    int nSize = m_simRingInfo.simRingLocationArry.size();

    //判断Name和Number是否为空，如果为空则返回
    if (!BWCallControl_CheckPhoneNumberValid(strPhoneNumber.c_str()))
    {
        return SetInvalidInfo(_LANG(TRID_INVALID_NUMBER));
    }
    else if (SRP_CheckNumberExit(strPhoneNumber.c_str()))
    {
        if (m_bInAdd || m_iLineID < 0 || m_iLineID >= nSize
                || m_simRingInfo.simRingLocationArry[m_iLineID].strAddress != strPhoneNumber.c_str())
        {
            return SetInvalidInfo(_LANG(TRID_NUMBER_ALREADY_EXISTS));
        }
    }

    if (m_bInAdd)
    {
        SSimRingLocation simRingLocation;
        simRingLocation.bAnswerConfirmationRequired = bAnswerConfirm;
        simRingLocation.strAddress = strPhoneNumber.c_str();

        m_simRingInfo.simRingLocationArry.push_back(simRingLocation);
    }
    else if (m_iLineID >= 0 && m_iLineID < nSize)
    {
        m_simRingInfo.simRingLocationArry[m_iLineID].bAnswerConfirmationRequired = bAnswerConfirm;
        m_simRingInfo.simRingLocationArry[m_iLineID].strAddress = strPhoneNumber.c_str();
    }

    if (m_bActiveWhenAdd)
    {
        m_simRingInfo.bActive = true;
    }

    //执行添加操作
    SRP_SetSimultaneousRing(m_simRingInfo);

    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), XSI_MSGBOX_ID_SAVE, -1,
                   MESSAGEBOX_Cancel);

    m_eMsgboxID = XSI_MSGBOX_ID_SAVE;

    return true;
}

void CSettinguiSimultaneousRingAddAndEditDeletae::OnCancleClick()
{
    SRP_CancelRequest();
}

bool CSettinguiSimultaneousRingAddAndEditDeletae::SetInvalidInfo(const yl::string & strNote)
{
    CSettingUIDataValidInfo validInfo;
    validInfo.SetValidIndex(INDEX_PHONE_NUMBER);
    validInfo.SetValidNote(strNote);
    g_SetUICallBackFun.DealWithValidData(validInfo);
    return false;
}

#endif //IF_FEATURE_BROADSOFT_PRIORITY_ALERT
