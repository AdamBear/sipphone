#ifndef __BSFT_CALL_LOG_CONTROLER_H__
#define __BSFT_CALL_LOG_CONTROLER_H__

#include "commondirctrl.h"

#ifndef DIR_SUPPORT_SINGLE_UIHELPER
class CBSFTCallLogUIHelper;
#endif


class CBSFTCallLogControler : public CCommonDirCtrl
{
public:
    CBSFTCallLogControler();
    virtual ~CBSFTCallLogControler();

public:
    static CBSFTCallLogControler * GetControler(CDirectoryControlerPtr & pControler);
    static BSFTCallLogRecord * GetBSFTCallLog(SmartPtr & pDetail);

#if IF_FEATURE_MUTI_CALLLOG
    static BSFTLogGroupRecord * GetBSFTCallLogGroup(SmartPtr & pDetail);
#endif

public:
    bool DialCallLog(int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    bool EditCallLog();
    bool IsPersonalCalllog();

    virtual bool PreLoadContact(int nGroupId);
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);

#if IF_FEATURE_MUTI_CALLLOG
    bool OnBack();

    virtual int GetAdjoinGroupId(bool bPrevious, int & nIndex);

    void SetCallType(CALL_TYPE eDirType);
    CALL_TYPE GetCallType();
#endif

    CBSFTCallLogUIHelper * GetBSFTCallLogUIHelper();

protected:
    void OnCopyContact(int nAction, int nId);

protected:
    bool m_bDownloading;
#if IF_FEATURE_MUTI_CALLLOG
    CALL_TYPE m_eDirType;
#endif
};

#endif
