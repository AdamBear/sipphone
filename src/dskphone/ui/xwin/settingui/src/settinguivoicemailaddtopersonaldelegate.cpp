#include "settinguivoicemailaddtopersonaldelegate.h"
#if IF_FEATURE_BROADSOFT_VOICEMAIL
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "commonapi/stringhelperapi.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "broadsoft/voicemail/include/voicemailcommon.h"
#include "broadsoft/voicemail/include/modvirtualvoicemail.h"
#include "contacts/directory/include/moddirinterface.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define INDEX_NAME 0
#define INDEX_NUMBER 1
#define NUMBER_LENGTH 23
}

CSettinguiVoiceMailAddToPersonalDelegate::CSettinguiVoiceMailAddToPersonalDelegate()
    : m_nVoiceMailIndex(-1)
{
}

CSettinguiVoiceMailAddToPersonalDelegate::~CSettinguiVoiceMailAddToPersonalDelegate()
{
}

CSettingUIDelegateBase *
CSettinguiVoiceMailAddToPersonalDelegate::CreateVoiceMailAddToPersonalDelegate()
{
    CSettinguiVoiceMailAddToPersonalDelegate * pDelegate = new CSettinguiVoiceMailAddToPersonalDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CSettinguiVoiceMailAddToPersonalDelegate::InitExtraData(void * pData)
{
    m_iLineID = vvMail_GetAccountId();
    if (NULL != pData)
    {
        int * pIndex = (int *)pData;
        if (NULL != pIndex)
        {
            m_nVoiceMailIndex = *pIndex;
        }
    }
}

CSettingUIPageDataBase * CSettinguiVoiceMailAddToPersonalDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL != m_pPageData && bReLoad)
    {
        sVoiceMailData stVoiceMailData;
        vvMail_GetDetailByIndex(stVoiceMailData, m_nVoiceMailIndex);

        yl::string strTitle = stVoiceMailData.m_bPrivacy ? _LANG(TRID_ANONYMOUS) :
                              stVoiceMailData.m_strName;
        m_pPageData->SetPageTitle(strTitle);

        CSettingUILineEditItemDataPtr pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(
                    GetItemByItemId(ADDTOPERSONAL_NAME));
        if (NULL != pEditItem)
        {
            pEditItem->SetValue(stVoiceMailData.m_strName);
            yl::string strDefauleIME = configParser_GetConfigString(kszContactDefaultIME).c_str();
            if (!strDefauleIME.empty())
            {
                pEditItem->SetDefaultIME(strDefauleIME);
            }
        }

        SetValue(ADDTOPERSONAL_NUMBER, stVoiceMailData.m_strNumber);
    }

    return m_pPageData;
}

bool CSettinguiVoiceMailAddToPersonalDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    yl::string strName = commonAPI_TrimString(GetValue(ADDTOPERSONAL_NAME), " ");
    yl::string strPhoneNumber = GetValue(ADDTOPERSONAL_NUMBER);

    if (strName.empty())
    {
        SetValidInfo(_LANG(TRID_DISPLAY_NAME_CANNOT_BE_EMPTY), INDEX_NAME);
        return false;
    }

    if (!BWCallControl_CheckPhoneNumberValid(strPhoneNumber.c_str(), NUMBER_LENGTH))
    {
        SetValidInfo(_LANG(TRID_NUMBER_INVALID), INDEX_NUMBER);
        return false;
    }

    ContactBroadSoftPersonal itemData;

    itemData.m_strDisplayName = strName;

    itemData.SetNumber(strPhoneNumber);

    int nRetCode = BsftDir_AddContact2Personal(itemData);

    if (nRetCode >= 0)
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_SAVING_FILES)
                                               , SETTING_MESSAGEBOX_Cancel, "", "",
                                               SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME, MSGBOX_ID_SAVE);
        return TRUE;
    }

    yl::string strTips;
    switch (nRetCode)
    {
    case DIR_RET_RERROR_NETWORK_ERROR:
        {
            strTips = _LANG(TRID_NETWORK_UNAVAILABLE);
        }
        break;
    case DIR_RET_CONTACT_EXIST:
        {
            strTips = _LANG(TRID_CONTACT_EXIST);
        }
        break;
    default:
        {
            strTips = _LANG(TRID_SAVE_FAILED);
        }
        break;
    }

    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), strTips
                                           , SETTING_MESSAGEBOX_Cancel, "", "",
                                           SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME, -1);

    return FALSE;
}

void CSettinguiVoiceMailAddToPersonalDelegate::SetValidInfo(const yl::string & strNote, int nIndex)
{
    CSettingUIDataValidInfo validInfo;
    validInfo.SetValidNote(strNote);
    validInfo.SetValidIndex(nIndex);
    validInfo.SetMsgBoxType(SETTING_MESSAGEBOX_Cancel);
    g_SetUICallBackFun.DealWithValidData(validInfo);
}

#endif //IF_FEATURE_BROADSOFT_VOICEMAIL
