#ifndef __LOCAL_CONTACT_CONTROLER_H__
#define __LOCAL_CONTACT_CONTROLER_H__

#include "commondirctrl.h"


class CLocalContactControler : public CCommonDirCtrl
{
public:
    CLocalContactControler();
    virtual ~CLocalContactControler();

public:
    static CLocalContactControler * GetControler(CDirectoryControlerPtr & pControler);
    static LocalDirContactRecord * GetLocalContact(SmartPtr & pDetail);
    static bool IsExitContact(yl::string & strName);
#ifdef DIR_SUPPORT_BATCH_OPERATE
    static void GetCopyDefaultContact(ContactGeneralItemData & itemData);
#endif
    // 校验联系人数据是否合法
    static bool CheckContactDataValid(const ContactGeneralItemData & itemData);
    static bool IsHadInvalidChar(const char * pszStr);

public:
// Group
    virtual bool IsGroupData(SmartPtr & pDetail);

    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual bool GetContactImage(int nId, yl::string & strImage) const;
#endif

    virtual int GetBrowseItemAmount();

    virtual LRESULT OnMessage(msgObject & refMessage);

    int GetContactAccountLine();

#ifdef DIR_SUPPORT_EXIST_SAVE
    bool IsContactChanged();
#endif

#if IF_FEATURE_FAVORITE_DIR
    bool IsFavoriteContact(int nContactId);
#endif

    int AddGroup(ContactGroupData & groupData, int nAction = DIR_ACTION_NONE);
    bool SaveGroup(int nGroupId, ContactGroupData & groupData, int nAction = DIR_ACTION_NONE);
    bool GetGroupData(int nGroupId, ContactGroupData & groupData);

protected:
    bool OnAddGroup();
    bool OnSaveGroup(int nGroupId);
    bool OnDelGroup(int nGroupId);
    bool OnClearGroup();
    bool OnAddContact();
    bool OnSaveContact();
    bool OnDelContact(int nContactId);
    bool OnClearContact();
    bool OnMoveToContact(int nContactId);
    bool OnMoveToBlacklist(int nContactId);
    bool OnCopyToContact(bool bOverwrite);
    bool OnCopyToBlacklist(bool bOverwrite);

    bool DeleteGroup(int nGroupId);
    bool ClearAllGroup();
    bool DeleteContact(int nContactId);
    bool ClearAllContact();
    bool MoveContact(int nContactId, int nSrcGroupId, int nTarGroupId);
#ifdef IF_FEATURE_FAVORITE_DIR
    bool OnCopyToFavorite(int nContactId);
    bool OnRemoveFromFavorite(int nContactId);
    bool CopyContactToFavorite(int nContactId);
    bool RemoveContactFromFavorite(int nContactId);
#endif

protected:

    bool CheckContactCopyNumber(int nContactId, ContactGeneralItemData & itemData);
    bool CheckGroupDataValid(const ContactGroupData & groupData);
    bool CheckChangeGroupSuccess(int nRetCode, int nAction);
    // 校验UI上取到的联系人数据是否合法(非法有提示)
#if IF_FEATURE_FAVORITE_DIR
    bool CheckFavoriteIndexValid();
#endif
    bool CheckContactValid(const ContactGeneralItemData & itemData);
    bool CheckChangeContactSuccess(int nGroupId, int nRetCode, int nAction = DIR_ACTION_NONE);
    bool RefreshSearchUI();
};

#endif
