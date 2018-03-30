#ifndef __LDAP_MANAGER_H__
#define __LDAP_MANAGER_H__

#include "taskaction/taskactioncallbackbase.h"

#include "contacts/ldaplogic/include/ldapdatadefine.h"
#include "contacts/directory/src/basecontactmanager.h"

class LdapManager : public CBaseContactManger
{
    // 搜索状态.
    enum SearchStatus
    {
        SEARCH_STATUS_INVALID = -1,  // 不正确的搜索状态.
        SEARCH_SUCCEED = 0,  // 搜索成功.
        SEARCHING = 1,  // 正在搜索.
        SEARCH_FAIL = 3,  // 搜索失败.
    };

    // 上次搜索数据.
    struct NowSearchData
    {
        yl::string m_strNowKey;  // 上次搜索关键字.
        SearchStatus m_eNowStatus;  // 上次搜索结果.

        NowSearchData()
        {
            Reset();
        }

        void Reset()
        {
            m_strNowKey.clear();
            m_eNowStatus = SEARCH_STATUS_INVALID;
        }
    };
public:

    static LdapManager & GetInstance() DECLARE_ATTR;

    void Init();

    // 处理配置更改的消息
    bool ProcessMessage(msgObject & refObj);

    virtual void OnTaskActionCallback(NS_TA::CTaskAction * pTaskAction);

    // 获取LDAP配置名.
    // 如果获取成功则返回配置的LDAP名, 否则返回空.
    yl::string GetName() const;

#if IF_FEATURE_34162
    // 重新设置LDAP配置
    void ReSetLdapOption();
#endif
    //获取超级搜索列表数据
    bool GetContactSuperSearchList(YLVector<SuperDirData> & output, const yl::string & strKey);

//涉及异步
public:

    bool ReqSearch(const yl::string & strKey);

    //获取联系人列表数据
    bool GetContactList(LdapListData * pData, const yl::string & strKey);

    bool GetContactInfoByKey(const yl::string & strKey, LdapContactData & stData);

protected:

    void SetLdapOption();

    void ClearData();

#if IF_FEATURE_34162
    void NotifSearchResult(int nRetCode, const yl::string & strKey);
#else
    void NotifSearchResult(bool isSuc, const yl::string & strKey);
#endif
    static void SetLogLevel(int iModuleID, int iLogLevel);

    LdapManager();

    virtual ~LdapManager();

#if IF_BUG_31829
    int GetContactCountsByName();
#endif

    void InitGroupData();

    virtual bool IsNeedShowAllGroup()
    {
        return false;
    }

private:

    NowSearchData m_stNowData;  // 搜索数据状态

    yl::string m_strSucKey; //搜索成功关键字

    bool m_bNeedSort;

    vLdapItem m_vLdapData;

    chCriticalSection m_ldapCs;
};


#define g_refLdapManager LdapManager::GetInstance()


#endif

