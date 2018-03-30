#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYTALKACTION_H__
#define __TALK_DSSKEY_DSSKEYTALKACTION_H__

/*
按键只发送TalkAction的Dsskey较多，直接抽象成同一个类
*/

#include "dsskey/include/idsskeyimplaction.h"
#include "dsskey/include/moddsskey.h"
#include "dsskey/include/dsskeystructure.h"
#include "dsskey/include/dsskey_def.h"


class CDsskeyTalkAction
    : public IDsskeyImplAction
{
public:
    CDsskeyTalkAction(DssKey_Type eDsskeyType, TALK_ACTION_TYPE eTalkActionType);

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

protected:
    virtual bool SendTalkAction();

private:
    TALK_ACTION_TYPE m_eTalkActionType;
};

#endif // __TALK_DSSKEY_DSSKEYTALKACTION_H__
#endif  //IF_FEATURE_DSSKEY
