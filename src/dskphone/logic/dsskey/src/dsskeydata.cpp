#include "dsskeydata.h"
#include "dsskey_common.h"

#include "dsskeyactionmanager.h"
#include "dsskeyaction.h"
#include "dsskeycommonfunc.h"
#include "dsskey_inner_enumtypes.h"
#include "dsskeydataentrymanager.h"
#include <xmlparser/xmlparser.hpp>
#include <configparser/modconfigparser.h>
#include <configiddefine.h>
#include <path_define.h>
#include <account/include/modaccount.h>
#include <account/include/accountmanager.h>
#include "moddsskey.h"
#include "dsskey_def.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "dsskeyuilogic/include/moddsskeyuilogic.h"

CFG_ITEM_MAP CDSSkeyData::s_mapLinekey;
CFG_ITEM_MAP CDSSkeyData::s_mapProgramkey;
CFG_ITEM_MAP CDSSkeyData::s_mapExpkey;

void CDSSkeyData::initDsskeyMapData()
{
    static bool bInited = false;


    if (bInited)
    {
        return;
    }

    // Linekey
    s_mapLinekey.insert(ATTR_TYPE, kszLinekeyType);
    s_mapLinekey.insert(ATTR_VALUE, kszLinekeyValue);
    s_mapLinekey.insert(ATTR_LABEL, kszLinekeyLabel);
    s_mapLinekey.insert(ATTR_LINE, kszLinekeyLine);
    s_mapLinekey.insert(ATTR_EXTENSION, kszLinekeyExtention);

    s_mapLinekey.insert(ATTR_SHORT_LABEL, kszLinekeyShortLabel);
    //s_mapLinekey.insert(ATTR_PICKUP_VALUE, kszLinekeyPickupValue);
    //s_mapLinekey.insert(ATTR_XML_PHONEBOOK, kszLinekeyXmlPhoneBook);

    //programkey
    s_mapProgramkey.insert(ATTR_TYPE, kszProgramkeyType);
    s_mapProgramkey.insert(ATTR_VALUE, kszProgramkeyValue);
    s_mapProgramkey.insert(ATTR_LABEL, kszProgramkeyLabel);
    s_mapProgramkey.insert(ATTR_LINE, kszProgramkeyLine);
    s_mapProgramkey.insert(ATTR_EXTENSION, kszProgramkeyExtention);

    //s_mapProgramkey.insert(ATTR_PICKUP_VALUE, kszProgramkeyPickupValue);
    //s_mapProgramkey.insert(ATTR_XML_PHONEBOOK, kszProgramkeyXmlPhoneBook);
    //s_mapProgramkey.insert(ATTR_HISTORY_TYPE, kszProgramkeyHistoryType);

    //expkey
    s_mapExpkey.insert(ATTR_TYPE, kszExpkeyType);
    s_mapExpkey.insert(ATTR_VALUE, kszExpkeyValue);
    s_mapExpkey.insert(ATTR_LABEL, kszExpkeyLabel);
    s_mapExpkey.insert(ATTR_LINE, kszExpkeyLine);
    s_mapExpkey.insert(ATTR_EXTENSION, kszExpkeyExtention);

    //s_mapExpkey.insert(ATTR_PICKUP_VALUE, kszExpkeyPickupValue);
    //s_mapExpkey.insert(ATTR_XML_PHONEBOOK, kszExpkeyXmlPhoneBook);
    //s_mapExpkey.insert(ATTR_HISTORY_TYPE, kszExpkeyHistoryType);

    bInited = true;
}

CFG_ITEM_MAP* CDSSkeyData::GetDsskeyConfigIDMapData(
    DSSKEY_MODULE_TYPE eModuleType)
{
    initDsskeyMapData();

    if (DMT_LINEKEY_MODULE == eModuleType)
    {
        return &s_mapLinekey;
    }
    else if (DMT_PROGRAMEKEY_MODULE == eModuleType)
    {
        return &s_mapProgramkey;
    }
    else if (IsEXPModule(eModuleType))
    {
        return &s_mapExpkey;
    }

    return NULL;
}

LPCSTR CDSSkeyData::GetConfigID(DSSKEY_MODULE_TYPE eModuleType, const char* szAttr)
{
    if (NULL == szAttr)
    {
        return "";
    }

    initDsskeyMapData();

    CFG_ITEM_MAP* pMap = GetDsskeyConfigIDMapData(eModuleType);

    if (NULL == pMap)
    {
        return "";
    }

    CFG_ITEM_MAP::iterator it = pMap->find(szAttr);

    if (pMap->end() == it)
    {
        return "";
    }

    return it->second;
}

yl::string CDSSkeyData::GetStringConfig(DSSKEY_MODULE_TYPE eModuleType, const char* szAttr,
                                        int iKeyIndex,
                                        int iModuleIndex, const yl::string& strDefValue,
                                        CONFIG_LEVEL_TYPE eLevel/* = CONFIG_LEVEL_IGNORE*/)
{
    LPCSTR strConfigID = GetConfigID(eModuleType, szAttr);

    if (textEMPTY(strConfigID))
    {
        return strDefValue;
    }

    iKeyIndex += 1;

    if (IsEXPModule(eModuleType) && iModuleIndex >= 0)
    {
        return configParser_GetCfgItemStringValue(strConfigID, iModuleIndex, iKeyIndex, eLevel);
    }

    return configParser_GetCfgItemStringValue(strConfigID, iKeyIndex, eLevel);
}

yl::string CDSSkeyData::GetStringConfig(int iDsskeyID, const char* szAttr, CONFIG_LEVEL_TYPE eLevel)
{
    DSSKEY_MODULE_TYPE eModuleType = GetModuleType(iDsskeyID);
    int iModuleIndex = GetModuleIndex(iDsskeyID);
    int iKeyIndex = GetDsskeyIndex(iDsskeyID);

    return GetStringConfig(eModuleType, szAttr, iKeyIndex, iModuleIndex, yl::string(), eLevel);
}

int CDSSkeyData::GetIntConfig(DSSKEY_MODULE_TYPE eModuleType, const char* szAttr, int iKeyIndex,
                              int iModuleIndex, int iDefValue, CONFIG_LEVEL_TYPE eLevel/* = CONFIG_LEVEL_IGNORE*/)
{
    LPCSTR strConfigID = GetConfigID(eModuleType, szAttr);

    if (textEMPTY(strConfigID))
    {
        return iDefValue;
    }

    iKeyIndex += 1;

    if (IsEXPModule(eModuleType) && iModuleIndex >= 0)
    {
        return configParser_GetCfgItemIntValue(strConfigID, iModuleIndex, iKeyIndex, eLevel);
    }

    return configParser_GetCfgItemIntValue(strConfigID, iKeyIndex, eLevel);
}

int CDSSkeyData::GetIntConfig(int iDsskeyID, const char* szAttr, CONFIG_LEVEL_TYPE eLevel)
{
    DSSKEY_MODULE_TYPE eModuleType = GetModuleType(iDsskeyID);
    int iModuleIndex = GetModuleIndex(iDsskeyID);
    int iKeyIndex = GetDsskeyIndex(iDsskeyID);

    return GetIntConfig(eModuleType, szAttr, iKeyIndex, iModuleIndex, -1, eLevel);
}

void CDSSkeyData::SetStringConfig(DSSKEY_MODULE_TYPE eModuleType, const char* szAttr,
                                  int iKeyIndex,
                                  int iModuleIndex, const yl::string& strValue, CONFIG_LEVEL_TYPE eLevel/* = CONFIG_LEVEL_PHONE*/)
{
    LPCSTR strConfigID = GetConfigID(eModuleType, szAttr);

    if (textEMPTY(strConfigID))
    {
        return;
    }

    iKeyIndex += 1;

    if (IsEXPModule(eModuleType) && iModuleIndex >= 0)
    {
        configParser_SetCfgItemStringValue(strConfigID, iModuleIndex, iKeyIndex, strValue, eLevel);
        return;
    }

    configParser_SetCfgItemStringValue(strConfigID, iKeyIndex, strValue, eLevel);
}

void CDSSkeyData::SetIntConfig(DSSKEY_MODULE_TYPE eModuleType, const char* szAttr, int iKeyIndex,
                               int iModuleIndex, int iDefValue, CONFIG_LEVEL_TYPE eLevel/* = CONFIG_LEVEL_PHONE*/)
{
    LPCSTR strConfigID = GetConfigID(eModuleType, szAttr);

    if (textEMPTY(strConfigID))
    {
        return;
    }

    iKeyIndex += 1;

    if (IsEXPModule(eModuleType) && iModuleIndex >= 0)
    {
        configParser_SetCfgItemIntValue(strConfigID, iModuleIndex, iKeyIndex, iDefValue, eLevel);
        return;
    }

    configParser_SetCfgItemIntValue(strConfigID, iKeyIndex, iDefValue, eLevel);
}

CDSSkeyData::CDSSkeyData(void)
{
    m_logicData.clear();
    m_bRelatedLock = false;
    m_iDsskeyID  = -1;
    m_chRemoteStatus = ' ';
    m_iRealLineID = -1;
    m_pExtFileds = NULL;
    timerID = 0;
    m_lineStatus = DS_NON;
}

CDSSkeyData::~CDSSkeyData(void)
{
    // 析构时，强制清空定时器
    killTimer();

    if (m_pExtFileds)
    {
        delete m_pExtFileds;
    }
}

bool CDSSkeyData::SetDsskeyAction(DssKey_Type dsskeyType)
{

    return true;
}


DSSKEY_MODULE_TYPE CDSSkeyData::getDsskeyModuleType()
{
    return m_logicData.eType ;
}

void CDSSkeyData::setDsskeyModuleType(DSSKEY_MODULE_TYPE type)
{

    m_logicData.eType = type;
}


yl::string CDSSkeyData::getExtFiledValue(const yl::string& strFiledName)
{
    if (m_pExtFileds)
    {
        YLHashMap<yl::string, yl::string>::iterator iter = m_pExtFileds->find(strFiledName);
        if (iter != m_pExtFileds->end())
        {

            return iter->second;
        }

    }
    return "";

}

UINT CDSSkeyData::getTimerID()
{
    return timerID;
}

void CDSSkeyData::setTimerID(UINT timID)
{
    timerID = timID;
}


void CDSSkeyData::setExtFieldValue(const yl::string& strFiledName,
                                   const yl::string& strFiledValue)
{
    if (m_pExtFileds == NULL)
    {

        m_pExtFileds = new YLHashMap<yl::string, yl::string>();
    }

    if (m_pExtFileds)
    {
        (*m_pExtFileds)[strFiledName] = strFiledValue;

    }
}

bool CDSSkeyData::ParserFromMemory(DSSKEY_MODULE_TYPE eModuleType, int iKeyIndex,
                                   int iModuleIndex/* = -1*/)
{
    DSSKEY_INFO("reload dsskey [%d] data beg", m_iDsskeyID);

    DssKey_Type eDKType = static_cast<DssKey_Type>(GetIntConfig(eModuleType, ATTR_TYPE, iKeyIndex,
                          iModuleIndex, -1));

    SetDsskeyType(eDKType);
    SetDsskeyAction(eDKType);

    SetValue(GetStringConfig(eModuleType, ATTR_VALUE, iKeyIndex, iModuleIndex, ""));

    SetLineIndex(GetIntConfig(eModuleType, ATTR_LINE, iKeyIndex, iModuleIndex, -1));

    SetLabel(GetStringConfig(eModuleType, ATTR_LABEL, iKeyIndex, iModuleIndex, ""));

    SetShortLabel(GetStringConfig(eModuleType, ATTR_SHORT_LABEL, iKeyIndex, iModuleIndex, ""));

    SetExtension(GetStringConfig(eModuleType, ATTR_EXTENSION, iKeyIndex, iModuleIndex, ""));

    SetLockType((DssKeyLockType) atoi(m_logicData.strValue.c_str()));

    DSSKEY_INFO("reload dsskey [%d] data end", m_iDsskeyID);

    CorrectRealBindAccount();
    return true;
}


bool CDSSkeyData::ToMemory(DSSKEY_MODULE_TYPE eModuleType, int iKeyIndex,
                           int iModuleIndex /* = -1 */, int iFlag/* = 0*/)
{
    DSSKEY_INFO("beg to save [%d] data", m_iDsskeyID);

    SetIntConfig(eModuleType, ATTR_TYPE, iKeyIndex, iModuleIndex, m_logicData.eDKType);

    SetStringConfig(eModuleType, ATTR_VALUE, iKeyIndex, iModuleIndex, m_logicData.strValue);

    SetIntConfig(eModuleType, ATTR_LINE, iKeyIndex, iModuleIndex, m_logicData.iLineIndex);

    if (!(iFlag & DSSKEY_SET_AUTO_LABEL))
    {
        SetStringConfig(eModuleType, ATTR_LABEL, iKeyIndex, iModuleIndex, m_logicData.strLabel);
    }

    SetStringConfig(eModuleType, ATTR_EXTENSION, iKeyIndex, iModuleIndex, m_logicData.strExtension);

    SetStringConfig(eModuleType, ATTR_SHORT_LABEL, iKeyIndex, iModuleIndex, m_logicData.strShortLabel);

    DSSKEY_INFO("end to save [%d] data", m_iDsskeyID);
    return true;
}


bool CDSSkeyData::CorrectDsskeyType(DSSKEY_MODULE_TYPE eModuleType, int iKeyIndex)
{
    if (dsskeyuilogic_CorrectDsskeyType(m_logicData, eModuleType, iKeyIndex))
    {
        _DsskeyManager.ModifyNativeData(m_iDsskeyID);
        return true;
    }

    return false;
}

int CDSSkeyData::GetDsskeyDataID()
{
    return m_iDsskeyID;
}

void CDSSkeyData::SetDSSKeyDataID(int dsskeyDataID)
{
    m_iDsskeyID = dsskeyDataID;
    m_logicData.iDssKeyID = dsskeyDataID;
    m_logicData.eType = GetModuleType(m_iDsskeyID);
}

void CDSSkeyData::GetLogicData(DssKeyLogicData& dataOutput)
{
    dataOutput = m_logicData;
    dataOutput.iDssKeyID = m_iDsskeyID;

    dataOutput.iStatus = (int)GetStatus();

    if (m_listStatus.size() > 0)
    {
        dataOutput.listAuxiliaryState.clear();
        YLList<Dsskey_Status>::iterator iter = m_listStatus.begin();
        for (; iter != m_listStatus.end(); iter++)
        {
            dataOutput.listAuxiliaryState.push_back((int)(*iter));
        }
    }


    if (dataOutput.strLabel.empty())
    {
        dataOutput.iTranslateLength = strlen(dsskey_GetDefaultLabel(GetDsskeyType()).c_str());
        //      dataOutput.strLabel = dsskey_GetDefaultLabel(GetDsskeyType());
    }
    else
    {

        dataOutput.iTranslateLength = 0;
    }

    // 只改变UI层数据，逻辑层还是原来的类型，Autoset不会重置其类型
    dsskeyuilogic_OnDsskeyLoad(dataOutput);
}

DssKeyLogicData& CDSSkeyData::GetLogicData()
{
    return m_logicData;
}

void CDSSkeyData::SetDsskeyType(DssKey_Type eType)
{
    DSSKEY_INFO("SetDsskeyType [%d] to [%d]", m_logicData.eDKType, eType);
    m_logicData.eDKType = eType;
}

#if IF_BUG_XMLBROWSER_EXP_LED
void CDSSkeyData::SetLightType(LIGHT_Type l_Type)
{
    m_logicData.eLightType = l_Type;
}

void CDSSkeyData::ClearLightType()
{
    m_logicData.eLightType = LT_NONE;
}
#endif

DssKey_Type CDSSkeyData::GetDsskeyType()
{
    return m_logicData.eDKType;
}

bool CDSSkeyData::OnKeyPress()
{
    return false;
}

bool CDSSkeyData::OnKeyLongPress()
{

    return false;
}

bool CDSSkeyData::UpdateStatus()
{
    dsskey_refreshDSSKeyUI(GetDsskeyDataID());
    return true;
}

bool CDSSkeyData::SetStatus(Dsskey_Status eStatus)
{
    if (IsValidStatus(eStatus))
    {
#if IF_BUG_XMLBROWSER_EXP_LED
        if (m_logicData.eDKType != DT_XMLBROWSER)
        {
            ClearLightType();
        }
#endif
        YLList<Dsskey_Status>::ListIterator iter = m_listStatus.begin();
        for (; iter != m_listStatus.end(); iter++)
        {
            Dsskey_Status oldStatus = getStatusBegin(*iter);
            Dsskey_Status newStatus = getStatusBegin(eStatus);
            if (oldStatus == newStatus && oldStatus != DS_NON)
            {
                if (*iter != eStatus)
                {
                    *iter = eStatus;
                    return true;
                }
                return false;
            }
        }
        m_listStatus.push_back(eStatus);
        return true;
    }
    else if (eStatus == DS_NON)
    {
        m_listStatus.clear();
        return true;
    }

	m_logicData.iStatus = GetStatus();
    return false;
}

int CDSSkeyData::GetLineIndex()
{
    return m_logicData.iLineIndex;
}

void CDSSkeyData::SetLabel(const yl::string& strLabel)
{
    DSSKEY_INFO("SetLabel [%s] to [%s]", m_logicData.strLabel.c_str(), strLabel.c_str());
    m_logicData.strLabel = strLabel;
}

void CDSSkeyData::SetShortLabel(const yl::string& strShortLabel)
{
    m_logicData.strShortLabel = strShortLabel;
}

void CDSSkeyData::SetValue(const yl::string& strValue)
{
    DSSKEY_INFO("SetValue [%s] to [%s]", m_logicData.strValue.c_str(), strValue.c_str());
    m_logicData.strValue = strValue;
}

void CDSSkeyData::SetExtension(const yl::string& strExtension)
{
    DSSKEY_INFO("SetExtension [%s] to [%s]", m_logicData.strExtension.c_str(), strExtension.c_str());
    m_logicData.strExtension = strExtension;
}

// 将blf状态转换成Dsskey_Status
Dsskey_Status CDSSkeyData::MapStatusToDsskeyStatus(int iStatus)
{
    return dsskey_MapStatusToDsskeyStatus((BLF_STATUS_CODE)iStatus);
}

bool CDSSkeyData::UpdateRemoteStatus(char chRemoteStatus)
{
    m_chRemoteStatus = chRemoteStatus;
    SetStatus(MapStatusToDsskeyStatus(chRemoteStatus));

    return false;
}

int CDSSkeyData::GetPickupStatus()
{
    // yzh状态待定
    return GetCallPickupStatus(m_chRemoteStatus);
}

void CDSSkeyData::GetValue(yl::string& strOutput)
{
    strOutput = m_logicData.strValue;
}

void  CDSSkeyData::GetLabel(yl::string& strOutput)
{
    strOutput = m_logicData.strLabel;
}

void CDSSkeyData::SetLineIndex(int lineIndex)
{
    DSSKEY_INFO("SetLineIndex [%d] to [%d]", m_logicData.iLineIndex, lineIndex);
    m_logicData.iLineIndex = lineIndex;
    m_logicData.iLineID =  lineIndex;
}


bool CDSSkeyData::SetLockType(DssKeyLockType lockType)
{
    DSSKEY_INFO("SetLockType [%d] to [%d]", m_logicData.eLockType, lockType);
    if (GetModuleType(m_iDsskeyID) == DMT_LINEKEY_MODULE && m_logicData.eDKType == DT_LINE)
    {
        m_logicData.eLockType = lockType;
        return true;
    }
    else
    {
        m_logicData.eLockType = DSS_KEY_LOCK_TYPE_DEFAULT;
    }
    return false;
}

DssKeyLockType CDSSkeyData::GetLockType()
{
    return m_logicData.eLockType;
}

void CDSSkeyData::SetLineStatus(Dsskey_Status eDsskeyStatus, Dsskey_Status eFirst,
                                Dsskey_Status eEnd)
{
    if (IsValidStatus(eDsskeyStatus))
    {
        YLList<Dsskey_Status>::iterator itr = m_listStatus.begin();
        YLList<Dsskey_Status>::iterator itrEnd = m_listStatus.end();
        for (; itr != itrEnd; itr++)
        {
            if (*itr >= eFirst
                    && *itr <= eEnd)
            {
                if (eFirst == *itr)
                {
                    // 移除掉状态
                    m_listStatus.erase(itr);
                }
                else
                {
                    // 替换已经存在的通话状态
                    *itr = eDsskeyStatus;
                }

                return;
            }
        }
        m_listStatus.push_back(eDsskeyStatus);
    }
}

void CDSSkeyData::RemoveLineStatus(Dsskey_Status eFirst, Dsskey_Status eEnd)
{
    YLList<Dsskey_Status>::iterator itr = m_listStatus.begin();
    YLList<Dsskey_Status>::iterator itrEnd = m_listStatus.end();
    for (; itr != itrEnd; itr++)
    {
        if (*itr >= eFirst
                && *itr <= eEnd)
        {
            m_listStatus.erase(itr);
            return;
        }
    }
}

void CDSSkeyData::ClearStatus()
{
    m_listStatus.clear();
}

void CDSSkeyData::SetRelatedLock(bool bValue)
{
    m_bRelatedLock = bValue;
}

Dsskey_Status CDSSkeyData::GetStatus()
{
    if (GetDsskeyType() == DT_LINE)
    {
        return m_lineStatus;
    }
    else
    {
        if (m_listStatus.size() > 0)
        {
            return m_listStatus.front();
        }
    }
    return DS_NON;
}

// 设置数据
void CDSSkeyData::SetData(const DssKeyLogicData& dsskeyLogicData)
{
    m_logicData = dsskeyLogicData;
    // 同步Line
    m_logicData.iLineID = m_logicData.iLineIndex;

    if (GetDsskeyType() == DT_LINE)
    {
        if (m_logicData.iLineIndex == -1)
        {
            m_iRealLineID = acc_GetUsableAccount(false);
        }
        else
        {
            m_iRealLineID = m_logicData.iLineIndex;
        }
    }
}

bool CDSSkeyData::IsStatusCtrlByRemote()
{
    DssKey_Type dsskeyType = GetDsskeyType();
    if (dsskeyType == DT_LINE)
    {
        return acc_IsBLAEnable(GetLineIndex());
    }

    return (dsskeyType == DT_BLFLIST
            || dsskeyType == DT_BLF
#ifdef IF_FEATURE_GENBAND
            || dsskeyType == DT_BLFGROUP
#endif
            || dsskeyType == DT_BLA
            || dsskeyType == DT_CALLPARK
            || (dsskeyType == DT_INTERCOM && configParser_GetConfigInt(kszIntercomLedEnable) != 0));
}

void CDSSkeyData::GetExtension(yl::string& strOutput)
{
    strOutput = m_logicData.strExtension;
}

void CDSSkeyData::GetStatusList(YLList<Dsskey_Status>& listOutput)
{
    listOutput.clear();
    listOutput = m_listStatus;
}

bool CDSSkeyData::IsLocked()
{
    return m_logicData.eLockType == DSS_KEY_LOCK_TYPE_LOCK &&  GetDsskeyType() == DT_LINE;
}

DSS_KEY_PAGE_BUTTON_PRIORITY CDSSkeyData::GetPageButtonPriority()
{
    Dsskey_Status dsskeyStatus = GetStatus();
    DSS_KEY_PAGE_BUTTON_PRIORITY eRet = DSS_KEY_LED_PRIORITY_LOWEST;
    switch (m_logicData.eDKType)
    {
    case DT_LINE:
        {
            eRet = GetLinePriority();
        }
        break;
    case DT_BLFLIST:
    case DT_BLF:
#ifdef IF_FEATURE_GENBAND
    case DT_BLFGROUP:
#endif
    case DT_INTERCOM:
        if (dsskeyStatus == DS_BLF_STATUS_RINGING)
        {
            eRet = DSS_KEY_LED_PRIORITY_HIGH;
        }

        break;
    case DT_CALLPARK:
        if (dsskeyStatus == DS_BLF_STATUS_TALKING)
        {
            eRet = DSS_KEY_LED_PRIORITY_HIGH;
        }
        break;
    default:
        break;
    }
    return eRet;
}

DSS_KEY_PAGE_BUTTON_PRIORITY CDSSkeyData::GetLinePriority()
{
    DSS_KEY_PAGE_BUTTON_PRIORITY eRet = DSS_KEY_LED_PRIORITY_LOWEST;
    Dsskey_Status eTalkStatu = DS_NON;
    Dsskey_Status eSCAStatu = DS_NON;
    Dsskey_Status eACDStatu = DS_NON;
    Dsskey_Status eLineStatu = DS_NON;
    YLList<Dsskey_Status>::iterator itr =  m_listStatus.begin();
    for (; itr != m_listStatus.end(); itr++)
    {
        if (*itr > DS_SESSION_FIRST
                && *itr < DS_SESSION_END)
        {
            eTalkStatu = (Dsskey_Status)(*itr);
        }
        else if (*itr > DS_BLF_FIRST
                 && *itr < DS_BLF_LAST)
        {
            eSCAStatu = (Dsskey_Status)(*itr);
        }
        else if (*itr > DS_ACD_FIRST
                 && *itr < DS_ACD_LAST)
        {
            eACDStatu = (Dsskey_Status)(*itr);
        }
        else if (*itr > DS_LINE_FIRST
                 && *itr < DS_LINE_END)
        {
            eLineStatu = (Dsskey_Status)(*itr);
        }
    }

    switch (eLineStatu)
    {
    case DS_LINE_REGISTERED:
        {
            eRet = DSS_KEY_LED_PRIORITY_SECOND_HIGH;
        }
        break;
    case DS_LINE_USING:
        {
            eRet = DSS_KEY_LED_PRIORITY_SECOND_HIGH;
            if (DS_SESSION_NORMAL_USING == eTalkStatu
                    || DS_SESSION_TALK == eTalkStatu) // 通话状态暂时和DS_SESSION_NORMAL_USING一样
            {
                eRet = DSS_KEY_LED_PRIORITY_LOWEST; // 非特殊状态优先级低
            }
            else if (DS_SESSION_HOLD == eTalkStatu)
            {
                eRet = DSS_KEY_LED_PRIORITY_LOWEST; // 非特殊状态优先级低

            }
            else if (DS_SESSION_DIAL == eTalkStatu)
            {
                eRet = DSS_KEY_LED_PRIORITY_LOWEST; // 非特殊状态优先级低
            }
        }
        break;

    default:
        break;
    }
    return  eRet;
}
int CDSSkeyData::GetRealLine()
{

    CorrectRealBindAccount();

    return m_iRealLineID;
}

//检查lineid的有效性，并处理auto的情况
void CDSSkeyData::CorrectRealBindAccount()
{
    if (GetDsskeyType() == DT_LINE)
    {
        if (m_logicData.iLineIndex >= 0)
        {
            m_iRealLineID = m_logicData.iLineIndex;
        }
        else
        {
            // 取第一个可用账号的ID
            m_iRealLineID = acc_GetUsableAccount(false);
        }
        // 外部未设置label才使用账号Label
//      if (m_logicData.strLabel.empty())
//      {
//          m_logicData.strLabel =  GetLabelFromAccount(m_iRealLineID);
//
        //UpdateLineStatus(MapLineState(acc_GetState(m_iRealLineID), m_iRealLineID));
    }
    //DSSKEY_INFO("m_iRealLineId = %d, m_strLabel = %s", m_iRealLineID, m_strLabel.c_str());
}

bool CDSSkeyData::OnTimer(UINT uTimerID)
{
    if (uTimerID == timerID)
    {
        CDSSKeyAction* pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(m_logicData.eDKType);
        if (pAction)
        {
            DssKeyLogicData data;
            GetLogicData(data);
            return pAction->OnTimer(data.iDssKeyID, uTimerID, data, NULL);
        }
    }

    return false;
}

void CDSSkeyData::SetTimer(UINT uTimeInterval)
{
    if (timerID == 0)
    {
        timerID = (UINT)(&m_logicData);
        timerSetThreadTimer(timerID, uTimeInterval);
        CDSSKeyAction* pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(m_logicData.eDKType);
        if (pAction)
        {
            DssKeyLogicData data;
            GetLogicData(data);
            return pAction->SetTimer(data.iDssKeyID, uTimeInterval, data, NULL);
        }
    }
}

int CDSSkeyData::setRemoteStatus(char remoteStatus)
{
    m_chRemoteStatus  = remoteStatus;
    return 0;
}

void CDSSkeyData::killTimer()
{
    if (timerID != 0)
    {
        timerKillThreadTimer(timerID);
        timerID = 0;
    }
}

bool CDSSkeyData::IsStatusExist(Dsskey_Status eStatus)
{
    if (IsValidStatus(eStatus))
    {
        YLList<Dsskey_Status>::ListIterator iter = m_listStatus.begin();
        for (; iter != m_listStatus.end(); iter++)
        {
            if (*iter == eStatus)
            {
                return true;
            }
        }
    }
    return false;
}

void CDSSkeyData::UpdateLineStatus(Dsskey_Status eDsskeyStatus)
{
    if (talklogic_IsDsskeyBindSession(m_iDsskeyID)
            && eDsskeyStatus > DS_LINE_FIRST && eDsskeyStatus < DS_LINE_END
            && eDsskeyStatus != DS_LINE_USING)
    {
        eDsskeyStatus = DS_LINE_USING;
    }

    m_lineStatus = eDsskeyStatus;
    SetStatus(eDsskeyStatus);
}

bool CDSSkeyData::OnTypeModify(DssKey_Type oldType, DssKey_Type newType, void* pExtraData)
{
    //类型未改变，直接返回
    if (oldType == newType)
    {
        return false;
    }

    // 类型改变，强制停掉定时器
    killTimer();

    //通知原类型
    CDSSKeyAction* pOldAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(oldType);
    if (pOldAction)
    {
        pOldAction->OnTypeModify(m_iDsskeyID, oldType, newType, pExtraData);
    }

    //通知新类型
    CDSSKeyAction* pNewAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(
                                    m_logicData.eDKType);
    if (pNewAction)
    {
        pNewAction->OnTypeModify(m_iDsskeyID, oldType, newType, pExtraData);
    }

    return true;
}

bool CDSSkeyData::OnConfigChange(void* pEtraData)
{
    CDSSKeyAction* pNewAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(
                                    m_logicData.eDKType);
    if (pNewAction)
    {
        bool bRet = pNewAction->OnConfigChange(m_iDsskeyID,  pEtraData);
        return bRet;
    }
    return false;
}



