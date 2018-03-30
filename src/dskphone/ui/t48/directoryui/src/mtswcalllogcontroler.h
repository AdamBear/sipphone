#if IF_FEATURE_METASWITCH_CALLLOG
#ifndef __MTSW_CALLLOG_CONTROLER_H__
#define __MTSW_CALLLOG_CONTROLER_H__

#include "commondirctrl.h"

#ifndef DIR_SUPPORT_SINGLE_UIHELPER
class CMTSWCallLogUIHelper;
#endif


class CMTSWCallLogControler : public CCommonDirCtrl
{
public:
    CMTSWCallLogControler();
    virtual ~CMTSWCallLogControler();

public:
    static CMTSWCallLogControler * GetControler(CDirectoryControlerPtr & pControler);
    static MTSWCallLogRecord * GetMTSWCallLog(SmartPtr & pDetail);

public:
    bool DialCallLog(int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    bool EditCallLog();

    virtual bool PreLoadContact(int nGroupId);
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);

    CMTSWCallLogUIHelper * GetMTSWCallLogUIHelper();

protected:
    void OnCopyContact(int nAction, int nId);

protected:
    bool m_bDownloading;
    //是否需要下载
    bool m_bIsNeedDownLoad;
};

#endif
#endif
