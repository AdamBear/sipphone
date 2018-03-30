#ifndef __BTUC_CONTACT_CONTROLER_H__
#define __BTUC_CONTACT_CONTROLER_H__


#include "directorycontroler.h"
#include "directorycommon.h"
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
#include "diruihelper.h"
#else
#if FEATURE_BROADTOUCH_UC
#include "presence/include/modpresence.h"
#endif
class CBTUCUIHelper;
#endif


class CBTUCContactControler : public CDirectoryControler
{
public:
    CBTUCContactControler();
    virtual ~CBTUCContactControler();

public:
    static CBTUCContactControler * GetControler(CDirectoryControlerPtr & pControler);
    static bool IsConfigure();
    static bool GetDialDetail(int nContactId, yl::string & strName, LIST_ATTRIBUTE & lsNum);
#if FEATURE_BROADTOUCH_UC
    static bool GetDetail(int nId, PresenceInfo & info);
#endif
    static bool IsGroupIdValid(int nGroupId);

public:
    virtual int GetParGroupId(int nGroupId);

    virtual bool PreSearchContact();
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
    virtual bool LoadListData(int nDataIndex, int nDataCount);
    virtual bool LoadDetailData(int nDataIndex, int nDataCount);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool GetContactName(yl::string & strName);
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual void GetDetailInfo(int & nId, int & nIndex, int & nCount) const;
    virtual bool GetContactImage(int nId, yl::string & strImage) const;
    virtual bool SwitchContact(int & nId, int & nIndex);
#endif
    virtual int GetAdjoinGroupId(bool bPrevious, int & nIndex);

    virtual LRESULT OnMessage(msgObject & refMessage);

    CBTUCUIHelper * GetBTUCUIHelper();
#if FEATURE_BROADTOUCH_UC
    bool GetGroupList(GroupList & listGroup);
#endif
    int GetGroupIndexById(int nGroupId);

    void SetContactId(int nContactId);
    int GetContactId() const;

protected:
    int m_nContactId;
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    int m_nDetailStatus;
    // 记录搜索关键字，用于详情界面切换时逻辑层的接口调用
    yl::string m_strSearchKey;
#endif
};
typedef CBTUCContactControler * CBTUCContactControlerPtr;

#endif
