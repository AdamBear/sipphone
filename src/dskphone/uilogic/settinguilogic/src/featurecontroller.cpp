#include "setting_inc.h"


using namespace yl;

// favorite保存数据
static LIST_FAVORITE_ITEM* s_pListFavoriteData;

bool GetDndData(DndData * pDndData, int iAccountID)
{
    // 全局
    if (-1 == iAccountID)
    {
        pDndData->m_bEnabled = configParser_GetConfigInt(kszDndStatus);
        pDndData->m_strOnCode = configParser_GetConfigString(kszDndOnCode);
        pDndData->m_strOffCode = configParser_GetConfigString(kszDndOffCode);
    }
    else
    {
        pDndData->m_bEnabled =  configParser_GetCfgItemIntValue(kszAccountDNDStatus, iAccountID) != 0;
        pDndData->m_strOnCode = configParser_GetCfgItemStringValue(kszAccountDNDOnCode, iAccountID);
        pDndData->m_strOffCode = configParser_GetCfgItemStringValue(kszAccountDNDOffCode, iAccountID);
    }

    return true;
}

void SetDndData(const DndData & refDndData, int iAccountID)
{
    bool bChanged = false;
    // DND,FWD不需要记录到mac-local

    // 全局
    if (-1 == iAccountID)
    {
        bChanged |= configParser_SetConfigInt(kszDndStatus, refDndData.m_bEnabled, CONFIG_LEVEL_PHONE);
        bChanged |= configParser_SetConfigString(kszDndOnCode, refDndData.m_strOnCode.c_str(),
                    CONFIG_LEVEL_PHONE);
        bChanged |= configParser_SetConfigString(kszDndOffCode, refDndData.m_strOffCode.c_str(),
                    CONFIG_LEVEL_PHONE);
    }
    else
    {
        bChanged |= configParser_SetCfgItemIntValue(kszAccountDNDStatus, iAccountID, refDndData.m_bEnabled,
                    CONFIG_LEVEL_PHONE);
        bChanged |= configParser_SetCfgItemStringValue(kszAccountDNDOnCode, iAccountID,
                    refDndData.m_strOnCode.c_str(), CONFIG_LEVEL_PHONE);
        bChanged |= configParser_SetCfgItemStringValue(kszAccountDNDOffCode, iAccountID,
                    refDndData.m_strOffCode.c_str(), CONFIG_LEVEL_PHONE);
    }

    if (bChanged)
    {
        etl_NotifyApp(TRUE, CONFIG_MSG_BCAST_CHANGED, ST_DND, 0);
    }
}

#if IF_BUG_27045
bool GetAreaCodeData(AreaCodeData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    pData->m_bEnable = (StrictCheck(configParser_GetConfigInt(kszAreaCodeEnable)) == CONFIG_ON);
    pData->m_strCode = configParser_GetConfigString(kszAreaCode);
    return true;
}

void SetAreaCodeData(const AreaCodeData & refData)
{
    ConfigSwitch eCurrentStatus = StrictCheck(configParser_GetConfigInt(kszAreaCodeEnable));
    ConfigSwitch eSwitchToSet = refData.m_bEnable ? CONFIG_ON : CONFIG_OFF;
    if (eCurrentStatus != eSwitchToSet)
    {
        // Save the value to config file.// need modify
        configParser_SetConfigInt(kszAreaCodeEnable, refData.m_bEnable ? 1 : 0, CONFIG_LEVEL_PHONE);
    }

    yl::string strCurrentCode = configParser_GetConfigString(kszAreaCode);
    if (strCurrentCode != refData.m_strCode)
    {
        configParser_SetConfigString(kszAreaCode, refData.m_strCode.c_str(), CONFIG_LEVEL_PHONE);
    }
}
#endif

bool GetAnonymousCallDataByLineId(AnonymousCallData * pData, int nLineId)
{
    if (pData == NULL)
    {
        return false;
    }

    // Read anonymous call status.
    pData->m_isAnonymousCallOn = configParser_GetCfgItemIntValue(kszAnonymousCallSwitch,
                                 nLineId) == CONFIG_ON;
    // Read anonymous code send or not.
    pData->m_isAnonymousSendCode = configParser_GetCfgItemIntValue(kszSendAnonymousCode,
                                   nLineId) == CONFIG_ON;
    // Read anonymous call on code.
    const yl::string strAnonyOnCode = configParser_GetCfgItemStringValue(kszAnonymousCallOnCode,
                                      nLineId);
    strncpy(pData->m_stCodeInfoAnonymousCall.m_szCodeOn,
            strAnonyOnCode.c_str(),
            min(sizeof(pData->m_stCodeInfoAnonymousCall.m_szCodeOn), strAnonyOnCode.length()));
    pData->m_stCodeInfoAnonymousCall.m_szCodeOn[knMaxCodeLength] = '\0';

    // Read anonymous call off code.
    const yl::string strAnonyOffCode = configParser_GetCfgItemStringValue(kszAnonymousCallOffCode,
                                       nLineId);
    strncpy(pData->m_stCodeInfoAnonymousCall.m_szCodeOff,
            strAnonyOffCode.c_str(),
            min(sizeof(pData->m_stCodeInfoAnonymousCall.m_szCodeOff), strAnonyOffCode.length()));
    pData->m_stCodeInfoAnonymousCall.m_szCodeOff[knMaxCodeLength] = '\0';

    // Read rejection status.
    pData->m_isRejectAnonymousCallOn = (configParser_GetCfgItemIntValue(kszRejectAnonymousCallSwitch,
                                        nLineId) == CONFIG_ON);

    // Read rejection send status.
    pData->m_isAnonymousRejectSendCode = (configParser_GetCfgItemIntValue(
            kszAccountSendAnonymousRejection, nLineId) == CONFIG_ON);

    // Read rejection on code.
    const yl::string strRejectOnCode = (configParser_GetCfgItemStringValue(kszRejectAnonymousCallOnCode,
                                        nLineId));
    strncpy(pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOn,
            strRejectOnCode.c_str(),
            min(sizeof(pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOn), strRejectOnCode.length()));
    pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOn[knMaxCodeLength] = '\0';

    // Read anonymous call off code.
    const yl::string strRejectOffCode = (configParser_GetCfgItemStringValue(
            kszRejectAnonymousCallOffCode, nLineId));
    strncpy(pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOff,
            strRejectOffCode.c_str(),
            min(sizeof(pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOff), strRejectOffCode.length()));
    pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOff[knMaxCodeLength] = '\0';
    return true;
}

void SetAnonymousCallDataByLineId(int nLineId, const AnonymousCallData & refData)
{
#if IF_BUG_21893
    configParser_SetCfgItemIntValue(kszAnonymousCallSwitch, nLineId,
                                    refData.m_isAnonymousCallOn ? CONFIG_ON : CONFIG_OFF, CONFIG_LEVEL_PHONE);

    configParser_SetCfgItemIntValue(kszSendAnonymousCode, nLineId,
                                    refData.m_isAnonymousSendCode ? CONFIG_ON : CONFIG_OFF, CONFIG_LEVEL_PHONE);

    // Set anonymous call on code.
    configParser_SetCfgItemStringValue(kszAnonymousCallOnCode, nLineId,
                                       refData.m_stCodeInfoAnonymousCall.m_szCodeOn, CONFIG_LEVEL_PHONE);

    // Set anonymous call off code.
    configParser_SetCfgItemStringValue(kszAnonymousCallOffCode, nLineId,
                                       refData.m_stCodeInfoAnonymousCall.m_szCodeOff, CONFIG_LEVEL_PHONE);
#else
    configParser_SetCfgItemIntValue(kszAnonymousCallSwitch, nLineId,
                                    refData.m_isAnonymousCallOn ? CONFIG_ON : CONFIG_OFF);

    configParser_SetCfgItemIntValue(kszSendAnonymousCode, nLineId,
                                    refData.m_isAnonymousSendCode ? CONFIG_ON : CONFIG_OFF);

    // Set anonymous call on code.
    configParser_SetCfgItemStringValue(kszAnonymousCallOnCode, nLineId,
                                       refData.m_stCodeInfoAnonymousCall.m_szCodeOn);

    // Set anonymous call off code.
    configParser_SetCfgItemStringValue(kszAnonymousCallOffCode, nLineId,
                                       refData.m_stCodeInfoAnonymousCall.m_szCodeOff);
#endif

// 发送. // need modify
//         if (g_pFeatureCode != NULL)
//         {
//             if (refData.m_isAnonymousCallOn)
//             {
//                 g_pFeatureCode->SendCodeToServer(nLineId,
//                     refData.m_stCodeInfoAnonymousCall.m_szCodeOn);
//             }
//             else
//             {
//                 g_pFeatureCode->SendCodeToServer(nLineId,
//                     refData.m_stCodeInfoAnonymousCall.m_szCodeOff);
//             }
//         }

    // Set rejection status.
#if IF_BUG_21893
    configParser_SetCfgItemIntValue(kszRejectAnonymousCallSwitch, nLineId,
                                    refData.m_isRejectAnonymousCallOn ? CONFIG_ON : CONFIG_OFF, CONFIG_LEVEL_PHONE);

    // Set rejection send status.
    configParser_SetCfgItemIntValue(kszAccountSendAnonymousRejection, nLineId,
                                    refData.m_isAnonymousRejectSendCode ? CONFIG_ON : CONFIG_OFF, CONFIG_LEVEL_PHONE);

    // Set rejection call on code.
    configParser_SetCfgItemStringValue(kszRejectAnonymousCallOnCode, nLineId,
                                       refData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn, CONFIG_LEVEL_PHONE);

    // Set rejection call off code.
    configParser_SetCfgItemStringValue(kszRejectAnonymousCallOffCode, nLineId,
                                       refData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff, CONFIG_LEVEL_PHONE);
#else
    configParser_SetCfgItemIntValue(kszRejectAnonymousCallSwitch, nLineId,
                                    refData.m_isRejectAnonymousCallOn ? CONFIG_ON : CONFIG_OFF);

    // Set rejection send status.
    configParser_SetCfgItemIntValue(kszAccountSendAnonymousRejection, nLineId,
                                    refData.m_isAnonymousRejectSendCode ? CONFIG_ON : CONFIG_OFF);

    // Set rejection call on code.
    configParser_SetCfgItemStringValue(kszRejectAnonymousCallOnCode, nLineId,
                                       refData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn);

    // Set rejection call off code.
    configParser_SetCfgItemStringValue(kszRejectAnonymousCallOffCode, nLineId,
                                       refData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff);
#endif

// code同步.need modify
//         if (g_pFeatureCode != NULL)
//         {
//             if (refData.m_isRejectAnonymousCallOn)
//             {
//                 g_pFeatureCode->SendCodeToServer(nLineId,
//                     refData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn);
//             }
//             else
//             {
//                 g_pFeatureCode->SendCodeToServer(nLineId,
//                     refData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff);
//             }
//         }

    //Notify Config Change
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nLineId);
    //异步通知账号配置改变 // need modify
    etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nLineId);
}

bool InitAnonymous()
{
    // Instantiate the model instance.
    AnonymousCallData stACD;
#define knMaxAccountNumber 4
    for (int i = 0; i < knMaxAccountNumber; ++i)
    {
        stACD.Reset();
        if (!GetAnonymousCallDataByLineId(&stACD, i))
        {
            continue;
        }

        // 发送同步.// need modify
        //                 if (g_pFeatureCode != NULL)
        //                 {
        //                     if (stACD.m_isAnonymousCallOn)
        //                     {
        //                         g_pFeatureCode->SendCodeToServer(
        //                             i, stACD.m_stCodeInfoAnonymousCall.m_szCodeOn);
        //                     }
        //                     else
        //                     {
        //                         g_pFeatureCode->SendCodeToServer(
        //                             i, stACD.m_stCodeInfoAnonymousCall.m_szCodeOff);
        //                     }
        //
        //                     if (stACD.m_isRejectAnonymousCallOn)
        //                     {
        //                         g_pFeatureCode->SendCodeToServer(
        //                             i, stACD.m_stCodeInfoRejectAnonymousCall.m_szCodeOn);
        //                     }
        //                     else
        //                     {
        //                         g_pFeatureCode->SendCodeToServer(
        //                             i, stACD.m_stCodeInfoRejectAnonymousCall.m_szCodeOff);
        //                     }
        //                 }
    }  //for.

    // 注册消息.// need modify
//  etl_NotifyApp(ACCOUNT_STATUS_CHANGED,
//                                  ACCOUNT_STATUS_CHANGED,
//                                  OnMessage);
    return true;
}

#if 0
// struct DSS key data, records DSS key data.
struct DssKeyData
{
    DssKey_Type m_eType;            // DSS key type.
    int m_iLineIndex;               // Line Index
    int m_iIndex;                   // Index Id.
    DssKeyEventType m_eEventType;   // Key Event type.
    yl::string m_strValue;  // Value.
    yl::string m_strExtension;  // Extension.
    yl::string m_strLabel;      // Label
    DssKeyLockType m_eLockType;     // Lock Type

    DssKeyData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = DT_NA;
        m_iLineIndex = -1;   // Line Index
        m_iIndex = -1;
        m_eEventType = DSS_KEY_EVENT_TYPE_CONFERENCE;
        m_strValue = "";
        m_strExtension = "";
        m_strLabel = "";
        m_eLockType = DSS_KEY_LOCK_TYPE_DEFAULT;
    }
};

bool GetDssKeyDataByIndex(DssKeyData * pData, int nDssKeyId, int eType)
{
    return true;
}

void SetDssKeyDataByIndex(const DssKeyData & refData, int nDssKeyId, int eType)
{

}

// Get Soft key data
bool GetProgramKeyDataByIndex(DssKeyData * pData, int nProgramKeyId)
{
#if 0
    char bufSection[20];
    sprintf(bufSection, "programablekey%d", nProgramKeyId + 1);
    HREGISTRY hSection = registryOpen(szDsskeyFile, bufSection);
    if (NULL == hSection)
    {
        SETTING_WARN("create section failed %s", bufSection);
        return false;
    }

    pData->m_eType = (DssKey_Type)registryGetInt(hSection, "DKtype", 0);
    pData->m_iLineIndex = registryGetInt(hSection, "Line", 0);
    pData->m_strValue = (LPCSTR)registryGetString(hSection, "Value", "");
    pData->m_strLabel = (LPCSTR)registryGetString(hSection, "Label", "");
    if (DT_LOCALGROUP == pData->m_eType
            || DT_IDLE_REMOTE == pData->m_eType
            || DT_BSGROUP == pData->m_eType
            || DT_NETWORKCALLLOG_SINGLE == pData->m_eType
            || DT_XMLGROUP == pData->m_eType)
    {
        pData->m_iIndex = registryGetInt(hSection, "XMLPhoneBook", 0);
    }
    else if (DT_IDLE_CALLLIST == pData->m_eType)
    {
        pData->m_iIndex = registryGetInt(hSection, "HistoryType", 0);
    }
    else if (DT_MEET_ME_CONFERENCE == pData->m_eType)
    {
        pData->m_strExtension = (LPCSTR)registryGetString(hSection, "PickupValue", "");
    }
#endif // #if 0
    return true;
}

// Set Soft key data
void SetProgramKeyDataByIndex(const DssKeyData & refData, int nProgramKeyId)
{

}
#endif

// load信息
BOOL LoadFavoriteData()
{
#ifdef PHONE_T6X
#define szFavoriteFile CONFIG_PATH"/favorite_setting.xml"
#else
#define szFavoriteFile SETTING_PATH"/favorite_setting.xml"
#endif

    if (NULL == s_pListFavoriteData)
    {
        s_pListFavoriteData = new LIST_FAVORITE_ITEM;
    }
    else
    {
        s_pListFavoriteData->clear();
    }

    xml_document doc;
    doc.load_file(szFavoriteFile);
    xml_node nodeRoot = doc.child("root_favorite_set");
    if (!nodeRoot)
    {
        return FALSE;
    }
    xml_node nodeChild = nodeRoot.child("item");
    if (!nodeChild)
    {
        return FALSE;
    }

    while (nodeChild)
    {
        FavoriteItem itemData;
        itemData.Reset();
        itemData.strId = nodeChild.attribute("id_name").value();
        itemData.strDisplayName = nodeChild.attribute("display_name").value();
        itemData.iPriority = nodeChild.attribute("priority").as_int();
        if (1 == nodeChild.attribute("enable").as_int()
                && (configParser_GetConfigInt(kszBroadsoftActive) != 0
                    || (itemData.strId != "networkcalllog" && itemData.strId != "broadsoftdirectory"
                        && itemData.strId != "networkdirectory")))
        {
            // 只加入可用的类型
            s_pListFavoriteData->push_back(itemData);
        }

        nodeChild = nodeChild.next_sibling("item");
    }

    // 对配置文件进行排序.
    Sort<FavoriteItem, FavoriteItem::FunCompare>(*s_pListFavoriteData, FavoriteItem::Compare);
    return TRUE;
}

// 获取favorite数据指针
LIST_FAVORITE_ITEM* GetFavoriteData()
{
    return s_pListFavoriteData;
}

#ifdef IF_FEATURE_AUTO_ANSWER_TIMER
void SetAutoAnswerData(const AutoAnswerTimer & refData)
{
    configParser_SetConfigInt(kszAutoAnswerDelay, refData.m_iTimer, CONFIG_LEVEL_PHONE);
}

int GetAutoAnswerData()
{
    int iTime = configParser_GetConfigInt(kszAutoAnswerDelay);
    if (iTime <= 0 || iTime > 4)
    {
        iTime = 3;
    }

    return iTime;
}
#endif
