#ifndef _TEXT_MESSAGE_INBOX_DELEGATE_H_
#define _TEXT_MESSAGE_INBOX_DELEGATE_H_

#include "textmessagebasedelegate.h"
#include "ylvector.h"

class CTextMessageInboxDelegate : public CTextMessageBaseDelegate
{
public:
    explicit CTextMessageInboxDelegate(int nGType = TEXT_MSG_GROUP_IN);
    virtual ~CTextMessageInboxDelegate();
    static CSettingUIDelegateBase * CreatTextMessageInboxDelegate();

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);

};

#endif //_TEXT_MESSAGE_INBOX_DELEGATE_H_
