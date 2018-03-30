#include "basemethod.h"
#include "commondata.h"
#include <ETLLib.hpp>
#include <devicelib/networkserver.h>


ContactRemoteMenu * Base2RemoteMenu(BaseDirDataRecord * pBase)
{
    // 转换为menu.
    if (pBase != NULL && (pBase->GetType() == DIR_TYPE_REMOTE_MENU
                          || pBase->GetType() == DIR_TYPE_REMOTE_HOTKEY))
    {
        return static_cast<ContactRemoteMenu *>(pBase);
    }
    return NULL;
}

ContactRemoteUnit * Base2Unit(BaseDirDataRecord * pBase)
{
    // 转换为具体联系人.
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_REMOTE_UNIT)
    {
        return static_cast<ContactRemoteUnit *>(pBase);
    }
    return NULL;
}


ContactRemoteHotKey * Base2HotKey(BaseDirDataRecord * pBase)
{
    // 转换为hot-key.
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_REMOTE_HOTKEY)
    {
        return static_cast<ContactRemoteHotKey *>(pBase);
    }
    return NULL;
}

ContactBroadSoftPersonal * Base2Personal(BaseDirDataRecord * pBase)
{
    // 转换为personal联系人.
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_BROADSOFT_ITEM_PERSONAL)
    {
        return static_cast<ContactBroadSoftPersonal *>(pBase);
    }
    return NULL;
}

ContactBroadSoftGroup * Base2Group(BaseDirDataRecord * pBase)
{
    // 转换为组联系人.
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_BROADSOFT_ITEM_GROUP)
    {
        return static_cast<ContactBroadSoftGroup *>(pBase);
    }
    return NULL;
}

ContactBroadSoftCommon * Base2Common(BaseDirDataRecord * pBase)
{
    // 转换为Common联系人.
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_BROADSOFT_ITEM_COMMON)
    {
        return static_cast<ContactBroadSoftCommon *>(pBase);
    }
    return NULL;
}

ContactBroadSoftEnterprise * Base2Enterprise(BaseDirDataRecord * pBase)
{
    // 转换为Enterprise联系人.
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE)
    {
        return static_cast<ContactBroadSoftEnterprise *>(pBase);
    }
    return NULL;
}

ContactBroadSoftCustom * Base2Custom(BaseDirDataRecord * pBase)
{
    // 转换为Enterprise联系人.
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_BROADSOFT_ITEM_CUSTOM)
    {
        return static_cast<ContactBroadSoftCustom *>(pBase);
    }
    return NULL;
}

ContactBroadSoftMenu * Base2BsftMenu(BaseDirDataRecord * pBase)
{
    // 转换为Enterprise联系人.
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_BROADSOFT_MENU)
    {
        return static_cast<ContactBroadSoftMenu *>(pBase);
    }
    return NULL;
}

#if IF_FEATURE_METASWITCH_DIRECTORY
ContactMetaSwitchMenu * Base2MtswMenu(BaseDirDataRecord * pBase)
{
    //
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_METASWITCH_MENU)
    {
        return static_cast<ContactMetaSwitchMenu *>(pBase);
    }
    return NULL;
}

ContactMetaSwitchContactList * Base2MtswContact(BaseDirDataRecord * pBase)
{
    //
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_METASWITCH_ITEM_CONTACTLIST)
    {
        return static_cast<ContactMetaSwitchContactList *>(pBase);
    }
    return NULL;
}


ContactMetaSwitchMLHG * Base2MtswhMLHGContact(BaseDirDataRecord * pBase)
{
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_METASWITCH_ITEM_MLHG)
    {
        return static_cast<ContactMetaSwitchMLHG *>(pBase);
    }
    return NULL;
}
#endif


LocalDirContactRecord * Base2LocalContact(BaseDirDataRecord * pBase)
{
    if (pBase != NULL && (pBase->GetType() == DIR_TYPE_LOCAL_CONTACT
                          || pBase->GetType() == DIR_TYPE_BT_CONTACT))
    {
        return static_cast<LocalDirContactRecord *>(pBase);
    }
    return NULL;
}

LocalLogItemRecord * Base2LocalLog(BaseDirDataRecord * pBase)
{
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_LOCALLOG_CALLLOG)
    {
        return static_cast<LocalLogItemRecord *>(pBase);
    }
    return NULL;
}

BSFTCallLogRecord * Base2BSFTLog(BaseDirDataRecord * pBase)
{
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_BSFT_CALLLOG)
    {
        return static_cast<BSFTCallLogRecord *>(pBase);
    }
    return NULL;
}

BSFTLogGroupRecord * Base2BSFTLogGroup(BaseDirDataRecord * pBase)
{
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_BSFT_LOG_GROUP)
    {
        return static_cast<BSFTLogGroupRecord *>(pBase);
    }
    return NULL;
}
#if IF_FEATURE_METASWITCH_CALLLOG
MTSWCallLogRecord * Base2MTSWLog(BaseDirDataRecord * pBase)
{
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_MTSW_CALLLOG)
    {
        return static_cast<MTSWCallLogRecord *>(pBase);
    }
    return NULL;
}

MTSWLogGroupRecord * Base2MTSWLogGroup(BaseDirDataRecord * pBase)
{
    if (pBase != NULL && pBase->GetType() == DIR_TYPE_MTSW_LOG_GROUP)
    {
        return static_cast<MTSWLogGroupRecord *>(pBase);
    }
    return NULL;
}
#endif

DirGroupRecord * Base2DirGroup(BaseDirDataRecord * pBase)
{
    if (pBase != NULL
            && pBase->m_eInheritType == DIR_INHERIT_GROUP)
    {
        return static_cast<DirGroupRecord *>(pBase);
    }
    return NULL;
}

BaseContactRecord * Base2BaseContact(BaseDirDataRecord * pBase)
{
    if (pBase != NULL
            && pBase->m_eInheritType == DIR_INHERIT_CONTACT)
    {
        return static_cast<BaseContactRecord *>(pBase);
    }
    return NULL;

}

#if IF_FEATURE_GBBOOK

ContactGBMenu * Base2GabMenu(BaseDirDataRecord * pBase)
{
    if (pBase != NULL
            && pBase->GetType() == DIR_TYPE_GB_MENU)
    {
        return static_cast<ContactGBMenu *>(pBase);
    }
    return NULL;
}


ContactGBBookUnit * Base2GbUnit(BaseDirDataRecord * pBase)
{
    if (pBase != NULL
            && pBase->GetType() == DIR_TYPE_GB_UNIT)
    {
        return static_cast<ContactGBBookUnit *>(pBase);
    }
    return NULL;
}

#endif
// TO DO-syl
bool IsNetworkConnected()
{
    return netIsNetworkReady();
}

CALL_TYPE GetLogTypeByGroupId(int nGroupId)
{
    if (nGroupId == kszPlacedLogId)
    {
        return CT_OUTGOING;
    }
    else if (nGroupId == kszReceivedLogId)
    {
        return CT_INCOMING;
    }
    else if (nGroupId == kszMissLogId)
    {
        return CT_MISSCALL;
    }
    else if (nGroupId == kszForWardedLogId)
    {
        return CT_FORWARDCALL;
    }
    else
    {
        return CT_UNKNOW;
    }
}

int GetFirstCharSortValue(const yl::string & strName)
{
    if (strName.empty())
    {
        return 35;
    }

    int nValue = strName[0];

    if (nValue >= 65 && nValue <= 90)
    {
        return nValue;
    }
    else if (nValue >= 97 && nValue <= 122)
    {
        return nValue - 32;
    }
    else
    {
        return 35;
    }
}

LdapContactData * Base2LdapContact(BaseDirDataRecord * pBase)
{
    if (pBase != NULL
            && pBase->GetType() == DIR_TYPE_LDAP)
    {
        return static_cast<LdapContactData *>(pBase);
    }
    return NULL;
}

int GetDirMaxDownloadSize()
{
    int nMaxFileSize = (1024 + 512) * 1024;
#if defined(IF_COMPILE_ARMDSPG) || defined(IF_COMPILE_ARM)
    nMaxFileSize = (512 + 256) * 1024;
#endif
    return nMaxFileSize;
}


