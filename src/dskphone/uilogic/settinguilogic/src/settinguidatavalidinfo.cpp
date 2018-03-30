#include "settinguidatavalidinfo.h"
#include "settinguicommondata.h"


CSettingUIDataValidInfo::CSettingUIDataValidInfo()
    : m_nValidIndex(-1)
    , m_strValidNote("")
    , m_nMsgBoxType(SETTING_MESSAGEBOX_NOTE)
    , m_nTime(NS_SET::g_SetPopNormalTime)
{
}

CSettingUIDataValidInfo::~CSettingUIDataValidInfo()
{
}

void CSettingUIDataValidInfo::Reset()
{
    m_nValidIndex = -1;
    m_strValidNote = "";
    m_nMsgBoxType = SETTING_MESSAGEBOX_NOTE;
    m_nTime = NS_SET::g_SetPopNormalTime;
}

int CSettingUIDataValidInfo::GetValidIndex()
{
    return m_nValidIndex;
}

void CSettingUIDataValidInfo::SetValidIndex(int iValidIndex)
{
    m_nValidIndex = iValidIndex;
}

const yl::string & CSettingUIDataValidInfo::GetValidNote()
{
    return m_strValidNote;
}

void CSettingUIDataValidInfo::SetValidNote(const yl::string & strValidNote)
{
    m_strValidNote = strValidNote;
}

int CSettingUIDataValidInfo::GetMsgBoxType()
{
    return m_nMsgBoxType;
}

void CSettingUIDataValidInfo::SetMsgBoxType(int nMsgBoxType)
{
    m_nMsgBoxType = nMsgBoxType;
}

int CSettingUIDataValidInfo::GetShowtime()
{
    return m_nTime;
}

void CSettingUIDataValidInfo::SetShowtime(int nShowTime)
{
    m_nTime = nShowTime;
}

