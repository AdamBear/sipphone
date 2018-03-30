#ifndef __BTUC_CONTACT_CONTROLER_H__
#define __BTUC_CONTACT_CONTROLER_H__


#include "directorycontroler.h"
#include "directorycommon.h"

#include "presence/include/presence_inc.h"

#if FEATURE_BROADTOUCH_UC
class CBTUCContactControler : public CDirectoryControler
{
public:
    CBTUCContactControler();
    virtual ~CBTUCContactControler();

public:
    static CBTUCContactControler* GetControler(CDirectoryControlerPtr& pControler);
    static bool IsConfigure();
    static bool GetDialDetail(int nContactId, yl::string& strName, LIST_ATTRIBUTE& lsNum);
    static bool GetDetail(int nId, PresenceInfo& info);

    static bool IsGroupIdValid(int nGroupId);

public:
    virtual int GetParGroupId(int nGroupId);

    virtual bool PreSearchContact();
#ifdef DIR_UI_SUPPORT_LOAD_DATA_LIST
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
    virtual bool LoadListData(int nDataIndex, int nDataCount);
#endif
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool GetContactShowName(yl::string& strName);
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual void GetDetailInfo(int& nId, int& nIndex, int& nCount) const;
    virtual bool GetContactImage(int nId, yl::string& strImage) const;
    virtual bool SwitchContact(int& nId, int& nIndex);
#endif

    virtual int GetBrowseItemAmount();
    virtual int GetSearchItemAmount();

    void PrepareDetailData();
    int GetContactIdByDataIndex(int nDataIndex);

    virtual LRESULT OnMessage(msgObject& refMessage);

    bool GetGroupList(GroupList& listGroup);
    bool GetGroupDataByIndex(int& nGroupID, yl::string& strName, int nDataIndex);
    int GetGroupIndexById(int nGroupId);
    int GetGroupIndexById(int nGroupId, GroupList& lsGroup);
    bool AdjustGroupIndex(bool bPrevious, int& nGroupId, int& nGroupIndex);

    void SetContactId(int nContactId);
    int GetContactId() const;

    int GetContactAmount();
    int GetContactIdByIndex(int nDataIndex);

protected:
    int m_nContactId;
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    // 记录搜索关键字，用于详情界面切换时逻辑层的接口调用
    yl::string m_strSearchKey;
#endif
};
typedef CBTUCContactControler* CBTUCContactControlerPtr;
#endif

#endif
