#include "modpresence.h"
#include "cpresencecontactmanager.h"
#include "uccontactdata.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "commonunits/modcommonunits.h"
#if FEATURE_BROADTOUCH_UC

void Presence_Init()
{
    if (DSK_PHONE_TYPE_COLOR != commonUnits_GetDskPhoneType())
    {
        return ;
    }
    g_objPresenceManager.Init();
}


//外部获取Group联系人的接口
bool Presence_GetListByGroupId(ContactBuddyListData * pData, GroupKey & keyGroup, int nLine,
                               const yl::string & strKey/* = ""*/, AttrType eSearchType/* = ATTR_ALL*/)
{
    return g_objPresenceManager.GetListByGroupId(pData, keyGroup, nLine, strKey, eSearchType);
}

//通过组ID，获取其下联系人数目，错误返回-1.
int Presence_GetChildCountByGroupId(GroupKey & keyGroup, PresenceType eType, int nLine/* = 0*/)
{
    return g_objPresenceManager.GetChildCountByGroupId(keyGroup, eType, nLine);
}

//通过id获取联系人详情
//refData.m_nLocal 中指定id
bool Presence_GetContactDetail(PresenceInfo & refData, PresenceType eType, int nLine/* = 0*/)
{
    return g_objPresenceManager.GetContactDetail(refData, eType, nLine);
}


//获取组链表显示
bool Presence_GetGroupList(PresenceType eType, GroupList & refList, int nLine/* = 0*/)
{
    return g_objPresenceManager.GetGroupList(eType, refList, nLine);
}

bool Presence_ReqChangPresence(_PRESENCE_TYPE eType)
{
    if (g_objPresenceManager.IsAllowChangePresenceStatus())
    {
        return g_objPresenceManager.ReqChangPresence(eType);
    }
    return false;
}

// 处理联系人消息.
// | refObj | 是联系人模块的消息.
// 返回LRESULT.
bool Presence_GetImageByNumber(yl::string & strPath, const yl::string & strNumber)
{
    return g_objPresenceManager.GetImageByNumber(strPath, strNumber);
}

bool Presence_GetNameByNumber(yl::string & strName, const yl::string & strNumber,
                              PresenceType eType, int nLine/* = 0*/)
{
    return g_objPresenceManager.GetNameByNumber(strName, strNumber, eType, nLine);
}

const PresenceInfo & Presence_GetLocalPresence()
{
    return g_pUCPresence->GetLocalPresence();
}

bool Presence_SuperSearch(const yl::string & strKey, YLVector<SuperDirData> & output)
{
    return g_pUCPresence->SuperSearch(strKey, output);
}

bool Presence_GetSuperDataById(const SuperDirData & refInIterm, DirectorySearchData & refData)
{
    return g_pUCPresence->GetSuperDataById(refInIterm, refData);
}

bool Presence_IsSearchHit(int nId, const yl::string & strSearchKey)
{
    return g_pUCPresence->IsSearchHit(nId, strSearchKey);
}


int Presence_MapStatus2DsskeyStatus(_PRESENCE_TYPE ePresenceType)
{
    int iStatus = DS_UC_OFFLINE;
    switch (ePresenceType)
    {
    case _PRESENCE_TYPE_ONLINE:
        iStatus = DS_UC_ONLINE;
        break;
    case _PRESENCE_TYPE_CHATTY:
        iStatus = DS_UC_CHATTY;
        break;
    case _PRESENCE_TYPE_AWAY:
        iStatus = DS_UC_AWAY;
        break;
    case _PRESENCE_TYPE_EXTENDED_AWAY:
        iStatus = DS_UC_EXTENDED_AWAY;
        break;
    case _PRESENCE_TYPE_BUSY:
        iStatus = DS_UC_BUSY;
        break;
    case _PRESENCE_TYPE_OFFLINE:
        iStatus = DS_UC_OFFLINE;
        break;
    case _PRESENCE_TYPE_UNKNOWN:
        iStatus = DS_UC_UNKNOWN;
        break;
    default:
        break;
    }
    return iStatus;
}

bool Presence_IsAllowChangePresenceStatus()
{
    return g_objPresenceManager.IsAllowChangePresenceStatus();
}


#endif //FEATURE_BROADTOUCH_UC

