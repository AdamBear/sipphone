#ifndef _EXP_LOGIC_MANAGER_HEADER_
#define _EXP_LOGIC_MANAGER_HEADER_

#if defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
#include "devicelib/phonedevice.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "exp/include/explogicdefine.h"
#include "edk/include/edk_def.h"
#include "edk/include/edkmsgdefine.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"

// http://10.2.1.199/Bug.php?BugID=99372
#define IS_EXP_SUPPORT_TRANSPARENCY 0

//CEXPLogicManager
class CEXPLogicManager : public CMKProcessor
{

public:
    CEXPLogicManager();
    virtual ~CEXPLogicManager();

public:
    virtual void Init() = 0;

    virtual void Uninit() = 0;

    virtual bool RefreshExp(int nExpIndex, bool bRefreshStatusInfo = true) = 0;

    virtual bool InitExpData() = 0;

    virtual bool OnExpInsert(int nExpIndex, DSSKEY_MODULE_TYPE eExpType) = 0;

    virtual bool OnExpRemove(int nExpIndex, DSSKEY_MODULE_TYPE eExpType) = 0;

    virtual yl::string GetDrawThreadName() = 0;

    virtual LRESULT OnEXPEvent(msgObject & msg) = 0;

public:
    int GetEXPNum() const;

    virtual bool GetUseEXPColor() const = 0;

    DEVICE_TYPE_DEF GetEXPType() const;

    DSSKEY_MODULE_TYPE EXPManager_GetEXPModuleType();

    bool IsEXPKey(int nDssKeyID);

    int GetMaxExpNum();

public:
    virtual int SetExpIcon(int nDsskeyID, int nIcon) = 0;
    virtual int SetExpLabel(int nDsskeyID, const char * pStrLabel) = 0;

    virtual int SetExpKeyDisp(int nDsskeyID, int nIcon, const char * pStrLabel, int nLedStatus,
                              int nDsskeyBgType, int nDisplayFlag /*=1*/, const char * pStrIcon = NULL,
                              int nStatus = 0, int nAuxiliaryStatus = 0) = 0;

    virtual int SetExpLEDStatus(int nDsskeyID, int nLedStatus) = 0;

    virtual int SetExpBKLight(int nExpIndex, int nBKLight) = 0;

    virtual int SetExpContrast(int nExpIndex, int nContrast) = 0;

    virtual int SetEXPBackGround(const char * pStrBackGround, int nTransparentLevel = 0,
                                 bool bRefresh = true) = 0;

    virtual int EnterExpScreenSaver(bool bEnter) = 0;

public:

    int RefreshStatusInfo();

    void Exp39Switch(int iExpIndex);

    bool IsExpKeyInCurrentPage(int nDsskeyID);

    void GetExpInfo(int dsskeyID, int & expIndex, int & rowIndex, int & pageIndex, int & colIndex);

public:
    virtual void ExpProcessReadyInit() = 0;

    virtual void ProcessEXPKeyEvent(int nDsskeyID, int nStatus) = 0;

    virtual void ProcessEXPPageEvent(int nEXPIndex, int nPage) = 0;

public:
    void AddEXPInfo(const ExpDisplayItemInfo & ExpData);

    void SendEXPInfo();

    void SetExpInfoSendTimer();

    void DestroyExpInfoSendTimer();

    BOOL OnExpInfoSendTimerOut(msgObject & objMessage);

private:
    ExpInfoList             m_listExpInfo;

    mkThreadTimer       m_objExpInfoSendTimer;

    int             m_nSendDelayCount;
};

#endif
#endif //_EXP_LOGIC_MANAGER_HEADER_
