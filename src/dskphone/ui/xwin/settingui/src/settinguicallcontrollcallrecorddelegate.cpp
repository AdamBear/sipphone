#include "settinguicallcontrollcallrecorddelegate.h"

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/callrecording/include/modcallrecording.h"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define MAX_PLAY_TONE_TIME 1800
#define MIN_PLAY_TONE_TIME 10
#define INDEX_PLAY_TONE_TIME 5
#define INVALID_MSGBOX_SHOW_TIME 2000
}

CSettinguiCallControllCallRecordDelegate::CSettinguiCallControllCallRecordDelegate()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiCallControllCallRecordDelegate::OnMessage);
}

CSettinguiCallControllCallRecordDelegate::~CSettinguiCallControllCallRecordDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiCallControllCallRecordDelegate::OnMessage);
}

CSettingUIDelegateBase *
CSettinguiCallControllCallRecordDelegate::CreateCallControllCallRecordDelegate()
{
    CSettinguiCallControllCallRecordDelegate * pDelegate = new CSettinguiCallControllCallRecordDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiCallControllCallRecordDelegate::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiCallControllCallRecordDelegate * pDelegate = (CSettinguiCallControllCallRecordDelegate *)
            g_pSettingUIManager->GetPageDelegate(kszPageIDCallControllCallRecord);
    if (NULL == pDelegate || pDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message)
    {
        if (XSI_CALL_RECORD_MODE_GET == msg.wParam)
        {
            MessageBox_RemoveMessageBox(pDelegate, MessageBox_OK, XSI_MSGBOX_ID_LOADING);

            pDelegate->RefreshtCallRecordData();

            g_SetUICallBackFun.RefreshUI(false, pDelegate->GetPageID());
        }
        else if (XSI_CALL_RECORD_MODE_PUT == msg.wParam)
        {
            MessageBox_RemoveMessageBox(pDelegate, MessageBox_OK, XSI_MSGBOX_ID_SAVE);
        }
        else
        {
            return FALSE;
        }
    }
    else if (XSI_MESSAGE_REQ_FAIL == msg.message)
    {
        pDelegate->OnMessageReqFail(msg);
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

CSettingUIPageDataBase * CSettinguiCallControllCallRecordDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        RefreshtCallRecordData();

        callRecord_QueryServerRecordMode(m_iLineID);

        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), XSI_MSGBOX_ID_LOADING, -1,
                       MESSAGEBOX_Cancel);

        m_eMsgboxID = XSI_MSGBOX_ID_LOADING;
    }

    return m_pPageData;
}

bool CSettinguiCallControllCallRecordDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    SCallRecord stRecordInfo;
    stRecordInfo.nRecordMode = GetComboboxValue(CALLRECORD_MODE);
    stRecordInfo.nPauseResumeMethod = GetComboboxValue(CALLRECORD_PAUSERESUMENOTE);
    stRecordInfo.bPlayStartStopAnnouncement = GetComboboxValue(CALLRECORD_PLAYSTARTSTOP);
    stRecordInfo.bRecordVM = GetComboboxValue(CALLRECORD_RECORDVM);
    stRecordInfo.bPlayTone = GetComboboxValue(CALLRECORD_PLAYTONE);
    stRecordInfo.strPlayToneTime = GetValue(CALLRECORD_PLAYTONETIME);

    int iToneTime = atoi(stRecordInfo.strPlayToneTime.c_str());
    // 判断Time是否合法
    if (!BWCallControl_CheckPhoneNumberValid(stRecordInfo.strPlayToneTime.c_str(), 10)
            || !(iToneTime >= MIN_PLAY_TONE_TIME && iToneTime <= MAX_PLAY_TONE_TIME))
    {
        CSettingUIDataValidInfo validInfo;
        validInfo.SetValidIndex(INDEX_PLAY_TONE_TIME);
        validInfo.SetValidNote(_LANG(TRID_INVALID_CALLRECORD_TIME));
        validInfo.SetShowtime(INVALID_MSGBOX_SHOW_TIME);
        g_SetUICallBackFun.DealWithValidData(validInfo);
        return false;
    }

    // 给服务器发送请求，保存结果
    callRecord_SetServerRecordMode(stRecordInfo, m_iLineID);

    m_eMsgboxID = XSI_MSGBOX_ID_SAVE;

    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), m_eMsgboxID, -1,
                   MESSAGEBOX_Cancel);

    return true;
}

void CSettinguiCallControllCallRecordDelegate::RefreshtCallRecordData()
{
    if (NULL != m_pPageData)
    {
        SCallRecord stRecordInfo;
        callRecord_GetServerRecordMode(stRecordInfo, m_iLineID);
        SetComboboxValue(CALLRECORD_MODE, stRecordInfo.nRecordMode);
        SetComboboxValue(CALLRECORD_PAUSERESUMENOTE, stRecordInfo.nPauseResumeMethod);
        SetComboboxValue(CALLRECORD_PLAYSTARTSTOP, stRecordInfo.bPlayStartStopAnnouncement);
        SetComboboxValue(CALLRECORD_RECORDVM, stRecordInfo.bRecordVM);
        SetComboboxValue(CALLRECORD_PLAYTONE, stRecordInfo.bPlayTone);
        SetValue(CALLRECORD_PLAYTONETIME, stRecordInfo.strPlayToneTime);
    }
}

void CSettinguiCallControllCallRecordDelegate::OnCancleClick()
{
    RO_CancelRequest();
}


#endif
