#ifndef __MOD_PRESENCE_H__
#define __MOD_PRESENCE_H__

#if FEATURE_BROADTOUCH_UC

#include "presence_inc.h"

void Presence_Init();


//外部获取Group联系人的接口
bool Presence_GetListByGroupId(ContactBuddyListData * pData, GroupKey & keyGroup, int nLine,
                               const yl::string & strKey = "", AttrType eSearchType = ATTR_TYPE_ALL);

//通过组ID，获取其下联系人数目，错误返回-1.
int Presence_GetChildCountByGroupId(GroupKey & keyGroup, PresenceType eType, int nLine = 0);

//通过id获取联系人详情
//refData.m_nLocal 中指定id
bool Presence_GetContactDetail(PresenceInfo & refData, PresenceType eType, int nLine = 0);


//获取组链表显示
bool Presence_GetGroupList(PresenceType eType, GroupList & refList, int nLine = 0);

bool Presence_ReqChangPresence(_PRESENCE_TYPE eType);

// 处理联系人消息.
// | refObj | 是联系人模块的消息.
// 返回LRESULT.
bool Presence_GetImageByNumber(yl::string & strPath, const yl::string & strNumber);

bool Presence_GetNameByNumber(yl::string & strName, const yl::string & strNumber,
                              PresenceType eType, int nLine = 0);

bool Presence_IsAllowChangePresenceStatus();


//返回本地自己的Presence信息
const PresenceInfo & Presence_GetLocalPresence();

//超级搜索专用接口
//-------------------------------------------------------/
bool Presence_SuperSearch(const yl::string & strKey, YLVector<SuperDirData> & output);

bool Presence_GetSuperDataById(const SuperDirData & refInIterm, DirectorySearchData & refData);

bool Presence_IsSearchHit(int nId, const yl::string & strSearchKey);

// Presence Status映射成Dsskey Status
int Presence_MapStatus2DsskeyStatus(_PRESENCE_TYPE ePresenceType);

//-------------------------------------------------------/

#endif //FEATURE_BROADTOUCH_UC

#endif
