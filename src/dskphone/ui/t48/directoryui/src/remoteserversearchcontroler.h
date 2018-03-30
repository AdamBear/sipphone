#ifndef __REMOTE_SERVER_SEARCH_CONTROLER_H__
#define __REMOTE_SERVER_SEARCH_CONTROLER_H__

#include "remotecontactcontroler.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
class CRemoteServerSearchUIHelper;
#endif


class CRemoteServerSearchControler : public CRemoteContactControler
{
public:
    CRemoteServerSearchControler();
    virtual ~CRemoteServerSearchControler();

    static CRemoteServerSearchControler * GetServerSearchControler(CDirectoryControlerPtr & pControler);

public:
    CRemoteServerSearchUIHelper * GetServerSearchUIHelper();

    int GetSearchGroupId() const;

    int GetPreGroupId();
    int GetNextGroupId();

    bool OnBack();

    virtual void OnEnterSearch();

    virtual bool LoadListData(int nDataIndex, int nDataCount);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);
    virtual int GetLogicListGroupId();

protected:
    void OnDownLoadFinished(msgObject & refMessage);

    void ClearSearchData();

    bool SwitchToGroup(int nGroupId, int nAction);

    virtual bool PreSearchContact();

protected:
    int m_nSearchGroupId;
};

#endif
