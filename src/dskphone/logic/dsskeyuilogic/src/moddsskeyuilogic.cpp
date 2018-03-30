#include "dsskey_inc.h"

void dsskeyuilogic_Init()
{
    _DsskeyUIData.Init();
    _DsskeyUIData.SaveFile(DSS_TEMP_FRAM_FILE);
}

bool dsskey_IsValidDsskeyType(int iDsskeyID, DssKey_Type eDsskeyType)
{
    DSSKEY_MODULE_TYPE moduleType = dsskey_GetModuleType(iDsskeyID);
    if (IsSupportModuleType(moduleType))
    {
        DsskeyPageInfo pageInfo;
        pageInfo.m_eUIType = CDsskeyUIData::MapMoudle2UI(moduleType);
        return _DsskeyUIData.IsSupportType(pageInfo, eDsskeyType);
    }

    return false;
}

const yl::string & dsskey_GetDefaultLabel(DssKey_Type eType)
{
    // 保持原有体验，NA不显示出来
    if (eType <= DT_NA || eType >= DT_LAST)
    {
        return _DsskeyUIData.GetDefaultData().strDisplayName;
    }

    return _DsskeyUIData.GetDefaultLabel(eType);
}

DssKey_Type dsskey_GetDssKeyType(const yl::string & strType)
{
    return _DsskeyUIData.GetDsskeyTypeByURI(strType);
}

CDsskeyItemAttr * dsskeyuilogic_GetAttrByType(DssKey_Type eType, DSSKEY_ATTR_ITEM eAttr)
{
    return _DsskeyUIData.GetItemAttr(eType, eAttr);
}

bool dsskeyuilogic_GetAllDsskey(const DsskeyPageInfo & pageInfo, VecCandidateValue & vecData,
                                bool bEventType)
{
    return _DsskeyUIData.GetAllDsskey(pageInfo, vecData, bEventType);
}

bool dsskeyuilogic_GetData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData)
{
    return _DsskeyUIData.GetData(pageInfo, dsskeyData);
}

bool dsskeyuilogic_UpdateData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData)
{
    DssKeyLogicData dsskeyLogicData;
    //dsskey_GetData(pageInfo.m_iDsskeyID, dsskeyLogicData);
    return _DsskeyUIData.GetExtraData(pageInfo, dsskeyData, dsskeyLogicData);
}

bool dsskeyuilogic_SaveData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData)
{
    return _DsskeyUIData.SaveData(pageInfo, dsskeyData);
}

bool dsskeyuilogic_CheckKeyEventSet(const DsskeyPageInfo & pageInfo, DssKey_Type eType)
{
    return (_DsskeyUIData.FindDsskeyInSet(pageInfo, eType, true)
            && _DsskeyUIData.FindDsskeyInSet(pageInfo, DT_KEYEVENT, false));
}

DSSKEY_READONLY_MASK dsskeyuilogic_IsDsskeyReadOnly(const DsskeyPageInfo & pageInfo,
        const DssKeyUIData & dsskeyData)
{
    return _DsskeyUIData.IsDsskeyReadOnly(pageInfo, dsskeyData);
}

bool dsskeyuilogic_IsDataValid(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData,
                               CDsskeyPair & errorTips)
{
    return _DsskeyUIData.IsDataValid(pageInfo, dsskeyData, errorTips);
}

yl::string dsskeyuilogic_GetContactLabel(const DssKeyLogicData & dsskeyLogicData)
{
    return _DsskeyUIData.GetContactLabel(dsskeyLogicData);
}

bool dsskeyuilogic_CorrectDsskeyType(DssKeyLogicData & logicData, DSSKEY_MODULE_TYPE eModuleType,
                                     int iKeyIndex)
{
    return _DsskeyUIData.CorrectDsskeyType(logicData, eModuleType, iKeyIndex);
}

bool dsskeyuilogic_OnDsskeyLoad(DssKeyLogicData & logicData)
{
    return !(_DsskeyUIData.CheckVisible(logicData));
}

bool dsskeyuilogic_IsNeedPassword(DssKey_Type eType)
{
    return _DsskeyUIData.IsNeedPassword(eType);
}
