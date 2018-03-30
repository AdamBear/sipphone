#ifndef __CLYNCCONTACT_MANAGER_H__
#define __CLYNCCONTACT_MANAGER_H__
#include "ETLLib.hpp"
#include "yllist.h"
#include "presence_inc.h"

#if FEATURE_BROADTOUCH_UC

class CBaseContactData;

class CPresenceContactManager : public CMKProcessor
{
public:

    static CPresenceContactManager & GetInstance();

    void Init();

    //外部获取Group联系人的接口
    bool GetListByGroupId(ContactBuddyListData * pData, GroupKey & keyGroup, int nLine,
                          const yl::string & strKey = "", AttrType eSearchType = ATTR_TYPE_ALL);

    //通过组ID，获取其下联系人数目，错误返回-1.
    int GetChildCountByGroupId(GroupKey & keyGroup, PresenceType eType, int nLine = 0);

    //通过id获取联系人详情
    //refData.m_nLocal 中指定id
    bool GetContactDetail(PresenceInfo & refData, PresenceType eType, int nLine = 0);


    //获取组链表显示
    bool GetGroupList(PresenceType eType, GroupList & refList, int nLine = 0);

    bool ReqChangPresence(_PRESENCE_TYPE eType);

    // 处理联系人消息.
    // | refObj | 是联系人模块的消息.
    // 返回LRESULT.
    bool GetImageByNumber(yl::string & strPath, const yl::string & strNumber);

    bool GetNameByNumber(yl::string & strName, const yl::string & strNumber, PresenceType eType,
                         int nLine = 0);

    LRESULT OnUCMessage(msgObject & refObj);

    // 处理计时器到时消息.
    BOOL OnTimeoutMessage(msgObject & refObj);

    bool IsAllowChangePresenceStatus();

    BOOL OnRefreshAllDelayTimer(msgObject & refObj);

private:
    CPresenceContactManager();

    ~CPresenceContactManager();

    //通过line返回对应的处理类
    CBaseContactData * GetCotactData(PresenceType eType, int nline = 0, bool IsCreat = false);

private:

    mkThreadTimer m_InitTimer;  // Init¼ÆÊ±Æ÷.
    mkThreadTimer m_RefreshAllDelayTimer;  // ÑÓ³Ù¸üÐÂËùÓÐÁªÏµÈË¼ÆÊ±Æ÷

};

#define g_objPresenceManager CPresenceContactManager::GetInstance()

#endif //FEATURE_BROADTOUCH_UC

#endif //__CLYNCCONTACT_MANAGER_H__

