#ifndef _TEXT_MESSAGE_OUTBOX_DELEGATE_H_
#define _TEXT_MESSAGE_OUTBOX_DELEGATE_H_

#include "textmessagebasedelegate.h"

class CTextMessageOutBoxDelegate : public CTextMessageBaseDelegate
{
public:
    explicit CTextMessageOutBoxDelegate(int nGType = TEXT_MSG_GROUP_SENDING);
    virtual ~CTextMessageOutBoxDelegate();
    static CSettingUIDelegateBase * CreatTextMessageOutBoxDelegate();

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);

};

#endif //_TEXT_MESSAGE_OUTBOX_DELEGATE_H_
