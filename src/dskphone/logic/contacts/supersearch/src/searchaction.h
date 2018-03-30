#ifndef __SEARCH_ACTION_H__
#define __SEARCH_ACTION_H__

#include "taskaction/taskaction.h"
#include "supersearchdatadefine.h"

class CSearchAction : public NS_TA::CTaskAction
{
public:
    // Override.
    virtual bool Exec();

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed)
    {
        return false;
    }

    void SetSearchKey(const yl::string & strKey);

    void SetNextSearchKey(const yl::string & strKey);

    yl::string GetSearchKey();

    yl::string GetNextSearchKey();

    yl::string GetLastSearchKey();

    bool NoNeedSearch();

    void SetLdapDataProcess(bool isOn, const yl::string & strLdapKey = "");

    bool IsProcessLdapData();

    //清楚缓存关键字
    void ResetLastKey();

    // 是否是ldap
    bool IsLdapPro()
    {
        return m_bIsLdapPro;
    }

public:

    CSearchAction();

    ~CSearchAction();

protected:

    void Search();

private:
    yl::string m_strSearchKey;

    yl::string m_strNextKey;

    yl::string m_strLastKey;

    bool m_bIsLdapPro;

    yl::string m_strLdapKey;

    SearchExeType m_exeType;

    chCriticalSection m_searchCs;

};
#endif
