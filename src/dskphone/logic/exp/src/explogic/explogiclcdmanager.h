#ifndef _EXP_LOGIC_LCD_MANAGER_H__
#define _EXP_LOGIC_LCD_MANAGER_H__
#include "explogicmanager.h"

#ifdef _EXP_BLACK_SUPPORT
class CEXPLogicLCDManager : public CEXPLogicManager
{
private:
    CEXPLogicLCDManager();

    ~CEXPLogicLCDManager();

    CEXPLogicLCDManager(const CEXPLogicLCDManager &);

public:
    virtual void Init();

    virtual void Uninit();

    virtual bool InitExpData();

public:
    virtual bool GetUseEXPColor() const;

    static CEXPLogicLCDManager & GetInstance();

    virtual bool OnExpInsert(int nExpIndex, DSSKEY_MODULE_TYPE eExpType);

    virtual bool OnExpRemove(int nExpIndex, DSSKEY_MODULE_TYPE eExpType);

    virtual LRESULT OnEXPEvent(msgObject & msg);

    virtual void ExpProcessReadyInit();

    virtual void ProcessEXPPageEvent(int nEXPIndex, int nPage);

    virtual void ProcessEXPKeyEvent(int nDsskeyID, int nStatus);

    virtual yl::string GetDrawThreadName();

public:
    virtual bool RefreshExp(int nExpIndex, bool bRefreshStatusInfo = true);

    virtual int SetExpIcon(int nDsskeyID, int nIcon);

    virtual int SetExpLabel(int nDsskeyID, const char * pStrLabel);

    virtual int SetExpKeyDisp(int nDsskeyID, int nIcon, const char * pStrLabel, int nLedStatus,
                              int nDsskeyBgType, int nDisplayFlag /*=1*/, const char * pStrIcon = NULL,
                              int nStatus = 0, int nAuxiliaryStatus = 0);

    virtual int SetExpLEDStatus(int nDsskeyID, int nLedStatus);

    virtual int SetExpBKLight(int nExpIndex, int nBKLight);

    virtual int SetExpContrast(int nExpIndex, int nContrast);

    virtual int SetEXPBackGround(const char * pStrBackGround, int nTransparentLevel = 0,
                                 bool bRefresh = true);

    virtual int EnterExpScreenSaver(bool bEnter);

private:
    void PackDisplayData(int nExpIndex, int nPageIndex, int nRowIndex, int nColIndex, int nIcon,
                         int nDisplayFlag, const char * szLableBuff, bool bLight, int nLedStatus);

    void PackLEDData(int nExpIndex, int nRowIndex, int nColIndex, int nLedStatus);

    void PackIconData(int nDsskeyID, int nIcon);
};
#endif
#endif   // !_EXP_LOGIC_LCD_MANAGER_H__



