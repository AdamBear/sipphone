#ifndef _SETTINGUI_DSSKEY_ATION_HEADER_
#define _SETTINGUI_DSSKEY_ATION_HEADER_

#include "dsskey/include/idsskeyimplaction.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "settingui/include/modsettingui.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/settinguidefine.h"

#if IF_FEATURE_METASWITCH
#include "messagebox/messageboxcallback.h"
#endif

class CMenuAciton: public IDsskeyImplAction
{
public:
    CMenuAciton(): IDsskeyImplAction(DT_IDLE_MENU)
    {
    }

    virtual ~CMenuAciton()
    {
    }

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};

class CStatusAction: public IDsskeyImplAction
{
public:
    CStatusAction(): IDsskeyImplAction(DT_IDLE_STATUS)
    {

    }

    virtual ~CStatusAction()
    {

    }

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};

class CDssKeyZero : public IDsskeyImplAction
{
public:
    CDssKeyZero() : IDsskeyImplAction(DT_ZERO) {}

    // 按键按下的处理
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};

#if IF_FEATURE_PAGING
class CDssKeyPagingList : public IDsskeyImplAction
{
public:
    CDssKeyPagingList(): IDsskeyImplAction(DT_PAGING_LIST)
    {

    }
    virtual ~CDssKeyPagingList()
    {
    }

    // 按键按下的处理
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
private:
    bool IsTopWndPaginglist();
};
#endif

class CDssKeySMS : public IDsskeyImplAction
{
public:
    CDssKeySMS() : IDsskeyImplAction(DT_SMS) {}
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};

class CDssKeyNewSMS : public IDsskeyImplAction
{
public:
    CDssKeyNewSMS() : IDsskeyImplAction(DT_IDLE_NEWSMS) {}
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};

class CDssKeyXMLBrowser : public IDsskeyImplAction
{
public:
    CDssKeyXMLBrowser() : IDsskeyImplAction(DT_XMLBROWSER) {}
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};

#if FEATURE_BROADTOUCH_UC
class CDssKeyMyStatus : public IDsskeyImplAction
{
public:
    CDssKeyMyStatus() : IDsskeyImplAction(DT_MY_STATUS) {}
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData&logicData, void*pExtraData);
};
#endif

#if IF_FEATURE_EDK
class CDssKeyEDK : public IDsskeyImplAction
{
public:
    CDssKeyEDK() : IDsskeyImplAction(DT_CUSTOM_KEY) {}
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL
class CDssKeyMessageList : public IDsskeyImplAction
{
public:
    CDssKeyMessageList() : IDsskeyImplAction(DT_MESSAGE_LIST) {}
    virtual ~CDssKeyMessageList() {}
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};
#endif

#if IF_FEATURE_METASWITCH
class CDssKeyMTSWLogout : public IDsskeyImplAction, CMsgBoxCallBackAction
{
public:
    CDssKeyMTSWLogout() : IDsskeyImplAction(DT_MTSW_LOGOUT)
    {

    }
    virtual ~CDssKeyMTSWLogout()
    {
    }

    virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox);
    // 按键按下的处理
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};

class CDssKeyMTSWLogIn : public IDsskeyImplAction
{
public:
    CDssKeyMTSWLogIn() : IDsskeyImplAction(DT_MTSW_LOGIN)
    {

    }
    virtual ~CDssKeyMTSWLogIn()
    {
    }

    // 按键按下的处理
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};
#endif

#endif

