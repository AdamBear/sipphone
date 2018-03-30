#include "modvoicemailuicommon.h"
#include "broadsoft/voicemail/include/modvirtualvoicemail.h"

bool VoiceMailUI_IsPrivacyVoiceMail(int nIndex)
{
    sVoiceMailData refData;
    vvMail_GetDetailByIndex(refData, nIndex);

    return refData.m_bPrivacy;
}