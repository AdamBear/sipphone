#include "settinguiaddlocationdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_ANYWHERE

#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/anywhere/include/anywherecontroller.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define PHONE_NUMBER_MAX_LENGTH 19
#define INDEX_PHONE_NUMBER 1
}

CSettinguiAddLocationDeletae::CSettinguiAddLocationDeletae()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END, &CSettinguiAddLocationDeletae::OnMessage);
}

CSettinguiAddLocationDeletae::~CSettinguiAddLocationDeletae()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiAddLocationDeletae::OnMessage);
}

CSettingUIDelegateBase * CSettinguiAddLocationDeletae::CreateAddLocationDelegate()
{
    CSettinguiAddLocationDeletae * pDelegate = new CSettinguiAddLocationDeletae;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiAddLocationDeletae::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiAddLocationDeletae * pDelegate = (CSettinguiAddLocationDeletae *)
            g_pSettingUIManager->GetPageDelegate(kszPageIDAnyWhereAddLocation);
    if (NULL == pDelegate || pDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message && XSI_ANYWHERE_ADD_LOCATION == msg.wParam)
    {
        MessageBox_RemoveMessageBox(pDelegate, MessageBox_OK, XSI_MSGBOX_ID_SAVE);
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

bool CSettinguiAddLocationDeletae::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    yl::string strName = GetValue(ANYWHERE_ADDLOCATION_NAME);
    yl::string strPhoneNumber = GetValue(ANYWHERE_ADDLOCATION_NUMBER);

    CSettingUIDataValidInfo validInfo;
    //判断输入的号码是否合法
    if (!BWCallControl_CheckPhoneNumberValid(strPhoneNumber.c_str(), PHONE_NUMBER_MAX_LENGTH))
    {
        return SetInvalidInfo(_LANG(TRID_NUMBER_INVALID));
    }
    //判断号码是否已存在
    if (AnyWhere_CheckNumberExit(strPhoneNumber.c_str()))
    {
        return SetInvalidInfo(_LANG(TRID_NUMBER_ALREADY_EXISTS));
    }

    SAYWLocation propSet;
    propSet.m_bActive = true;
    propSet.m_strPhoneNum = strPhoneNumber;
    propSet.m_strDescription = strName;
    propSet.m_bEnableDiversionInhibitor = false;
    propSet.m_bRequireAnswerConfirmation = false;
    propSet.m_bUseBbasedCallControlServices = false;

    //执行添加操作
    AnyWhere_RequestAddLocation(propSet);

    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), XSI_MSGBOX_ID_SAVE, -1,
                   MESSAGEBOX_Cancel);

    m_eMsgboxID = XSI_MSGBOX_ID_SAVE;

    return true;
}

void CSettinguiAddLocationDeletae::OnCancleClick()
{
    AnyWhere_CancelAllRequest();
}

bool CSettinguiAddLocationDeletae::SetInvalidInfo(const yl::string & strNote)
{
    CSettingUIDataValidInfo validInfo;
    validInfo.SetValidIndex(INDEX_PHONE_NUMBER);
    validInfo.SetValidNote(strNote);
    validInfo.SetMsgBoxType(SETTING_MESSAGEBOX_Cancel);
    g_SetUICallBackFun.DealWithValidData(validInfo);
    return false;
}

#endif //IF_FEATURE_BROADSOFT_ANYWHERE
