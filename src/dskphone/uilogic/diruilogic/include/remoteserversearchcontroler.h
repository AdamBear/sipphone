#ifndef __REMOTE_SERVER_SEARCH_CONTROLER_H__
#define __REMOTE_SERVER_SEARCH_CONTROLER_H__

#include "remotecontactcontroler.h"


class CRemoteServerSearchControler : public CRemoteContactControler
{
public:
    CRemoteServerSearchControler();
    virtual ~CRemoteServerSearchControler();

    static CRemoteServerSearchControler * GetServerSearchControler(CDirectoryControlerPtr & pControler);

public:
    virtual void UnInit();

    virtual void EnterState(int nState);

    virtual bool IsManualSearch() const;

#if DIR_UI_FEATURE_LOAD_DATA_LIST
    virtual bool LoadListData(int nDataIndex, int nDataCount);
#endif
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);
    virtual int GetResultGroupId();

    int GetSearchGroupId() const;
    int GetPreGroupId();
    int GetNextGroupId();

protected:
    void OnDownLoadFinished(msgObject & refMessage);

    void ClearSearchData();

    bool SwitchToGroup(int nGroupId, bool bPreGroup);

    virtual bool PreSearchContact();

protected:
    int m_nSearchGroupId;
};

#endif
