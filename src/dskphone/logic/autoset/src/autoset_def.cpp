#include "autoset_inc.h"

void autoset_dsskeydata_t::clear()
{
    strLabel.clear();
    strValue.clear();
    strExtension.clear();
    strShortLabel.clear();
    eDKType = DT_NA;
    iLineIndex = -1;
    iStatus = -1;
    strKey.clear();
}

autoset_dsskeydata_t & autoset_dsskeydata_t::operator<<(const DssKeyLogicData &
        tDsskeyData)
{
    iDssKeyID = tDsskeyData.iDssKeyID;
    eDKType = tDsskeyData.eDKType;
    iLineIndex = tDsskeyData.iLineIndex;
    strLabel = tDsskeyData.strLabel;
    strValue = tDsskeyData.strValue;
    strExtension = tDsskeyData.strExtension;
    strShortLabel = tDsskeyData.strShortLabel;
    return *this;
}

autoset_dsskeydata_t & autoset_dsskeydata_t::operator>>(DssKeyLogicData & tDsskeyData)
{
    tDsskeyData.eDKType = eDKType;
    tDsskeyData.iLineIndex = iLineIndex;
    tDsskeyData.strLabel = strLabel;
    tDsskeyData.strValue = strValue;
    tDsskeyData.strExtension = strExtension;
    tDsskeyData.strShortLabel = strShortLabel;
    tDsskeyData.iStatus = iStatus;
    // id不处理
    return *this;
}

autoset_dsskeydata_t & autoset_dsskeydata_t::operator<<(BLFBaseData & tBlfData)
{
    strKey = tBlfData.m_strKey;
    strExtension = tBlfData.m_strExten;
    strValue = tBlfData.m_strNumber;
    strLabel = tBlfData.m_strDisplayName.empty() ? tBlfData.m_strNumber : tBlfData.m_strDisplayName;
    iStatus = tBlfData.GetShowStatus();
    return *this;
}

autoset_dsskeydata_t & autoset_dsskeydata_t::operator<<(LocalDirContactRecord & record)
{
    iLineIndex = record.m_nLine;
    strExtension = commonAPI_FormatString("%d", record.m_nId);
    strLabel = record.m_strDisplayName;

    if (!record.GetNumberByName(kszOfficeNumber, strValue) || strValue.empty())
    {
        if (!record.GetNumberByName(kszMobileNumber, strValue) || strValue.empty())
        {
            record.GetNumberByName(kszOtherNumber, strValue);
        }
    }

    return *this;
}

autoset_dsskeydata_t & autoset_dsskeydata_t::operator<<(DssKey_Type eType)
{
    eDKType = eType;
    return *this;
}

autoset_dsskeydata_t & autoset_dsskeydata_t::operator<<(int iLineID)
{
    iLineIndex = iLineID;
    return *this;
}
