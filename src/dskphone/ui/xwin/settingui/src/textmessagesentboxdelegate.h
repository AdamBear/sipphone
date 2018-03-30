#ifndef _TEXT_MESSAGE_SENTBOX_DELEGATE_H_
#define _TEXT_MESSAGE_SENTBOX_DELEGATE_H_

#include "textmessagebasedelegate.h"

class CTextMessageSentBoxDelegate : public CTextMessageBaseDelegate
{
public:
    explicit CTextMessageSentBoxDelegate(int nGType = TEXT_MSG_GROUP_SENT);
    virtual ~CTextMessageSentBoxDelegate();
    static CSettingUIDelegateBase * CreatTextMessageSentBoxDelegate();

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);

};

#endif //_TEXT_MESSAGE_SENTBOX_DELEGATE_H_
