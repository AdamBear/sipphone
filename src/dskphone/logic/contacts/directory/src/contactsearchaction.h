#ifndef __CONTACT_SEARCH_ACTION_H__
#define __CONTACT_SEARCH_ACTION_H__

#include "taskaction/taskaction.h"
#include "basecontactmanager.h"


class CContactSearchAction : public CTaskAction
{
public:

    CContactSearchAction(CBaseContactManger * pManger);

    virtual ~CContactSearchAction();

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

    YLVector<int> & GetSearchResult();

    void SetSearchKey(const yl::string & strKey);

    const yl::string & GetSearchKey();

    void SetSearchGroupId(int nId);

    void ReSet();

public:

    bool m_bIsNeedNotif;


private:

    YLVector<int> m_vSearchResult;

    CBaseContactManger * m_pManager;

    yl::string m_strSearchKey;

    yl::string m_strNextKey;

    yl::string m_strLastKey;

    int m_nGroupId;

    int m_nLastGroupId;

    SearchMethod m_eSearchMethod;

};

#endif
