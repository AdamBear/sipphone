#ifndef _TEXT_MESSAGE_DRAFTBOX_DELEGATE_H_
#define _TEXT_MESSAGE_DRAFTBOX_DELEGATE_H_

#include "textmessagebasedelegate.h"

class CTextMessageDraftBoxDelegate : public CTextMessageBaseDelegate
{
public:
    explicit CTextMessageDraftBoxDelegate(int nGType = TEXT_MSG_GROUP_DRAFT);
    virtual ~CTextMessageDraftBoxDelegate();
    static CSettingUIDelegateBase * CreatTextMessageDraftBoxDelegate();

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);

};

#endif //_TEXT_MESSAGE_DRAFTBOX_DELEGATE_H_
