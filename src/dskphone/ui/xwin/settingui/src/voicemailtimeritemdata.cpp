#include "voicemailtimeritemdata.h"

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL

CVoiceMailTimerItemData::CVoiceMailTimerItemData()
    : CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE)
{
    m_eItemType |= LIST_ITEM_TYPE_VOICE_MAIL_TIMER;
}

CVoiceMailTimerItemData::~CVoiceMailTimerItemData()
{
}

int CVoiceMailTimerItemData::GetVoiceMailTimerDataItemType()
{
    return (LIST_ITEM_TYPE_VOICE_MAIL_TIMER | SETTING_ITEM_TYPE_BASE);
}

CVoiceMailTimerItemData* CVoiceMailTimerItemData::GetVoiceMailTimerDataItem(
    CSettingUIItemDataBase* pDataItem)
{
    if (NULL == pDataItem
            || CVoiceMailTimerItemData::GetVoiceMailTimerDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CVoiceMailTimerItemDataPtr)pDataItem;
}

CSettingUIItemDataBase* CVoiceMailTimerItemData::CreateVoiceMailTimerDataItem()
{
    CVoiceMailTimerItemDataPtr pVoiceMailTimerItemData = new CVoiceMailTimerItemData();

    return pVoiceMailTimerItemData;
}

const yl::string& CVoiceMailTimerItemData::GetStatusIcon()
{
    return m_strIcon;
}

void CVoiceMailTimerItemData::SetStatusIcon(const yl::string& strIcon)
{
    m_strIcon = strIcon;
}

const yl::string& CVoiceMailTimerItemData::GetStatusText()
{
    return m_strStatusText;
}

void CVoiceMailTimerItemData::SetStatusText(const yl::string& strStatusText)
{
    m_strStatusText = strStatusText;
}

const yl::string& CVoiceMailTimerItemData::GetTotalTimeString()
{
    return m_strTotal;
}

void CVoiceMailTimerItemData::SetTotalTimeString(const yl::string& strTotal)
{
    m_strTotal = strTotal;
}

int CVoiceMailTimerItemData::GetPlayStatus()
{
    return m_nPlayStatus;
}

void CVoiceMailTimerItemData::SetPlayStatus(int nStatus)
{
    m_nPlayStatus = nStatus;
}

bool CVoiceMailTimerItemData::IsIconShow()
{
    return m_bShowIcon;
}

void CVoiceMailTimerItemData::SetIconShow(bool bShow)
{
    m_bShowIcon = bShow;
}

bool CVoiceMailTimerItemData::IsStatusTextShow()
{
    return m_bShowStatusText;
}

void CVoiceMailTimerItemData::SetStatusTextShow(bool bShow)
{
    m_bShowStatusText = bShow;
}

long CVoiceMailTimerItemData::GetTimerMaxTicks()
{
    return m_lTickTime;
}

void CVoiceMailTimerItemData::SetTimerMaxTicks(long lTickTime)
{
    m_lTickTime = lTickTime;
}
#endif

