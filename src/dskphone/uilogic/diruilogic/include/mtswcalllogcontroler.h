#ifndef __MTSW_CALLLOG_CONTROLER_H__
#define __MTSW_CALLLOG_CONTROLER_H__

#if IF_FEATURE_METASWITCH_CALLLOG
#include "commondirctrl.h"

class CMTSWCallLogControler : public CCommonDirCtrl
{
public:
    CMTSWCallLogControler();
    virtual ~CMTSWCallLogControler();

public:
    static CMTSWCallLogControler* GetControler(CDirectoryControlerPtr & pControler);
    static MTSWCallLogRecord* GetMTSWCallLog(SmartPtr& pDetail);

public:
    virtual void SetGroupId(int nGroupId);
    virtual int GetBrowseItemAmount();
    bool DialCallLog(int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    bool EditCallLog();

    virtual bool PreLoadContact(int nGroupId);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject& refMessage);
    virtual void PrepareDetailData();
    virtual bool IsGroupData(SmartPtr& pDetail);

protected:
    void OnCopyContact(int nAction, int nId);

protected:
    bool m_bDownloading;
    bool m_bIsNeedDownLoad;
};

#endif

#endif //IF_FEATURE_METASWITCH_CALLLOG
