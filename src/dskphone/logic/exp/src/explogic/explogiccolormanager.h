#ifndef _EXP_LOGIC_COLOR_MANAGER_H__
#define  _EXP_LOGIC_COLOR_MANAGER_H__

#include "explogicmanager.h"

#ifdef _EXP_COLOR_SUPPORT
enum EXP50_INIT_STATUS
{
    EXP50_STATUS_NONE = 0,
    EXP50_STATUS_INITING = 1,
    EXP50_STATUS_INITED = 2
};

class CEXPLogicColorManager : public CEXPLogicManager
{
private:

    CEXPLogicColorManager();
    ~CEXPLogicColorManager();
    CEXPLogicColorManager(const CEXPLogicColorManager &);

public:

    static CEXPLogicColorManager & GetInstance();

    virtual void Init();

    virtual void Uninit();

    virtual bool OnExpInsert(int nExpIndex, DSSKEY_MODULE_TYPE eExpType);

    virtual bool OnExpRemove(int nExpIndex, DSSKEY_MODULE_TYPE eExpType);

    virtual void ProcessEXPPageEvent(int nEXPIndex, int nPage);

    virtual void ProcessEXPKeyEvent(int nDsskeyID, int nStatus);

    virtual bool RefreshExp(int nExpIndex, bool bRefreshStatusInfo = true);

    virtual bool InitExpData();

    virtual LRESULT OnEXPEvent(msgObject & msg);

public:

    virtual int SetExpIcon(int nDsskeyID, int nIcon);

    virtual int SetExpLabel(int nDsskeyID, const char * pStrLabel);

    virtual int SetExpKeyDisp(int nDsskeyID, int nIcon, const char * pStrLabel, int nLedStatus,
                              int nDsskeyBgType, int nDisplayFlag /*=1*/, const char * pStrIcon = NULL,
                              int nStatus = 0, int nAuxiliaryStatus = 0);

    virtual int SetExpLEDStatus(int nDsskeyID, int nLedStatus);

    virtual int SetExpBKLight(int nExpIndex, int nBKLight);

    virtual int SetEXPBackGround(const char * pStrBackGround, int nTransparentLevel = 0,
                                 bool bRefresh = true);

    virtual int EnterExpScreenSaver(bool bEnter);

    virtual int SetExpContrast(int nExpIndex, int nContrast);

    virtual yl::string GetDrawThreadName();

    virtual bool GetUseEXPColor() const;

public:

    void ResetEXPLight(int nIndex);

    void ShowEXPPageLight(int nIndex);

    int SetExpLEDStatusByIndex(int nExpIndex, int nKeyIndex, int nLedStatus);

    void EXPFillWithColor(const EXP_FILL_COLOR & objExpColor, int nIndex = 0);

    void EXPFillWithPicture(const char * pStrPicture, int nIndex = 0);

    void EXPDrawText(const char * pStrText, int nIndex = 0);

    void EXPShowLogo(int nIndex = 0);

    void SetEditingText(const char * pStrLabel);

    void SendEditingText();

    int GetLedTypeByLedRuleColorType(int nLedRuleColorType);

    void ExpProcessReadyInit();

    void SetEditingKey(bool bEdit, int nDsskeyID = -1);

public:

    void ProcessEDKEvent(LedRuleData * pEXPLightList, int nCount);

    void PageKeyEDKFlushSwitch(bool bStart);

    void PageKeyEDKFlushEnable(bool bEnable);

    void EXPBackLightUpdate(int nLightLevel, int nIndex = -1);

    void EXPPowerSavingEnable(bool bEnable);

public:

    //将刷新信息加到队列中
    void AddEXPLightData(int nExpIndex, int nKeyIndex, int nLedStatus);

    void AddEXPUpdateData(int nDssKeyID
                          , const char * pStrLabel, int nIcon, int nLedStatus, int nDisplayFlag
                          , bool bPaintIcon = true, bool bPaintText = true, bool bLightOnly = false
                          , const char * pStrIcon = NULL, int nStatus = 0
                          , bool bSetAuxiliaryStatus = false, int nAuxiliaryStatus = 0,
                          int nDsskeyBgType = EXP_DSS_GROUNDPHOTO_MODE_NORMAL);

    //在消息队列中添加刷新一整页的消息标志
    void AddExpFlashPageInfo(int nExpIndex);

    void RecordAuxiliaryStatus(int nExpIndex, int nPage, int nKeyIndex, int nStatus);

public:

    void SetPageFlushTimer();

    void DestroyPageFlushTimer();

    BOOL OnPageFlushTimerOut(msgObject & objMessage);

    void setCloseDelayTimer();

    void destroyCloseDelayTimer();

    BOOL OnCloseDelayTimerOut(msgObject & objMessage);


    void SetIsXwindow(bool bXwindow);

public:

#if IS_EXP_SUPPORT_TRANSPARENCY
    void ChangeWidgetTransparency();
#endif

private:

    void RunExpProcess();
    EXP50_INIT_STATUS GetExpInitStatus(int iIndex);

    void SetExpInitStatus(int iIndex, EXP50_INIT_STATUS eStatus);

private:

    EXP50_INIT_STATUS             m_nExpStatus[MAX_EXP_NUM];
    int             m_nPressedDsskeyID;
    int             m_nEditingKeyIndex;

    char                m_szLabel[EXP_LABEL_MAX_SIZE];

    bool                m_bExpProcessFirstRun;
    bool                m_bExpProcessUninit;

    //当前是否有接exp
    bool                m_bExpExist;

    mkThreadTimer       m_objCloseDelayTimer;
    mkThreadTimer       m_objPageFlushTimer;

    int             m_aPageStatus[MAX_EXP_NUM][EXP50_PAGEKEY_COUNT];
    int             m_aKeyStatus[MAX_EXP_NUM][EXP50_PAGEKEY_COUNT][EXP_PAGE_SIZE];
    bool                m_bPageKeyFlushing;

    YLVector<LedRuleData> m_listPageFlushRule;
    int             m_nFlushStep;
    int             m_nPageFlushTime;
    bool                m_bPageFlushing;
    bool                m_bPageFlushEnable;

    bool                m_nExpPowerSavingEnable;


////////////////xwindow exp/////////////////////////////
    bool            m_bXwindow;




};
#endif
#endif  // !_EXP_LOGIC_COLOR_MANAGER_H__

