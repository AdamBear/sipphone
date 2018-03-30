#ifndef _EXP_50_PROCESS_H__
#define  _EXP_50_PROCESS_H__
#include "baseexpmsgprocess.h"
#ifdef _EXP_COLOR_SUPPORT

#define EXP_50_PAGE_KEY_ANTI_SHAKE_TIME 200

class CExp50MsgProcess : public CBaseExpMsgProcess
{
public:

    CExp50MsgProcess();

    ~CExp50MsgProcess();

    virtual void KeyProcess(int iKeyStatus, msgObject & msg);

    virtual inline DEVICE_TYPE_DEF  GetMsgProcessType() const
    {
        return DT_EXP_50;
    }

    virtual void ExpPluginProcess(msgObject & msg);

    virtual bool OnTimer(msgObject & msg);

private:

    inline DsskeyID GetRegDsskeyID()
    {
        return m_DssId;
    }

    inline void RegisterDsskeyID(DsskeyID dssId)
    {
        m_DssId = dssId;
    }

	bool SetPageKeyTimer();

	bool KillPageKeyTimer();

	bool OnPageKeyTimer();
private:

    bool m_bPageKeyCheck;
    int m_nCheckCount;
    int m_nLastCode;
    UINT m_uPageKeyAntiShakeTimer;
};

#endif
#endif //!_EXP_50_PROCESS_H__


