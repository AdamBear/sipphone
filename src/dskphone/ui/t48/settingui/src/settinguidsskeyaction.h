#ifndef _SETTINGUI_DSSKEY_ATION_HEADER_
#define _SETTINGUI_DSSKEY_ATION_HEADER_

#include "dsskey/include/idsskeyimplaction.h"
#include "dsskey/include/dsskey_enumtypes.h"

class CMenuAciton: public IDsskeyImplAction
{
public:
    CMenuAciton(): IDsskeyImplAction(DT_IDLE_MENU) {}
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

class CStatusAction: public IDsskeyImplAction
{
public:
    CStatusAction(): IDsskeyImplAction(DT_IDLE_STATUS) {}

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

class CDssKeyZero : public IDsskeyImplAction
{
public:
    CDssKeyZero(): IDsskeyImplAction(DT_ZERO) {}

    // 按键按下的处理
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

class CDssKeySMS : public IDsskeyImplAction
{
public:
    CDssKeySMS() : IDsskeyImplAction(DT_SMS) {}
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

class CDssKeyNewSMS : public IDsskeyImplAction
{
public:
    CDssKeyNewSMS() : IDsskeyImplAction(DT_IDLE_NEWSMS) {}
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

class CDssKeyPagingList : public IDsskeyImplAction
{
public:
    CDssKeyPagingList() : IDsskeyImplAction(DT_PAGING_LIST) {}
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
private:
    bool IsTopWndPaginglist();
};

#if IF_FEATURE_METASWITCH
class CDssKeyMTSWLogIn : public IDsskeyImplAction
{
public:
    CDssKeyMTSWLogIn(): IDsskeyImplAction(DT_MTSW_LOGIN)
    {

    }
    virtual ~CDssKeyMTSWLogIn()
    {
    }

    //virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox);

    // 按键按下的处理
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};

class CDssKeyMTSWLogout : public IDsskeyImplAction
{
public:
    CDssKeyMTSWLogout(): IDsskeyImplAction(DT_MTSW_LOGOUT)
    {

    }
    virtual ~CDssKeyMTSWLogout()
    {
    }

    // 按键按下的处理
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL
class CDssKeyMessageList : public IDsskeyImplAction
{
public:
    CDssKeyMessageList(): IDsskeyImplAction(DT_MESSAGE_LIST) {}
    virtual ~CDssKeyMessageList() {}
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);
};
#endif
#endif

