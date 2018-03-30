#ifndef __LOCAL_CALL_LOG_CONTROLER_H__
#define __LOCAL_CALL_LOG_CONTROLER_H__

#include "commondirctrl.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
class CLocalCallLogUIHelper;
#endif


class CLocalCallLogControler : public CCommonDirCtrl
{
public:
    CLocalCallLogControler();
    virtual ~CLocalCallLogControler();

public:
    static CLocalCallLogControler * GetControler(CDirectoryControlerPtr & pControler);
    static LocalLogItemRecord * GetLocalCallLog(SmartPtr & pDetail);

public:
    bool DeleteCallLog(int nCallLogId);
    bool ClearAllCallLog();

    bool DialCallLog(int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    bool EditCallLog();
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual bool GetContactImage(int nId, yl::string & strImage) const;
#endif

public:
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool GetContactName(yl::string & strName);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);

protected:
    void OnCopyContact(int nAction, int nId);

    CLocalCallLogUIHelper * GetLocalCallLogUIHelper();
};

#endif
