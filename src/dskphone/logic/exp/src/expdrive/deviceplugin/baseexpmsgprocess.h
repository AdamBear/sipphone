#ifndef _BASE_EXP_MSG_PROCESS_H__
#define _BASE_EXP_MSG_PROCESS_H__

#if defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
#include "exp/src/expdrive/expdriveheader/expscannerdefine.h"
#include "exp/include/explogicdefine.h"
#include "exp_struct.h"
#include "dsklog/log.h"
class CBaseExpMsgProcess
{
public:

    CBaseExpMsgProcess();
    virtual ~CBaseExpMsgProcess();

public:

    virtual void KeyProcess(int iKeyStatus, msgObject & msg) = 0;
    virtual void ExpPluginProcess(msgObject & msg);
    virtual void ExpRemoveProcess(msgObject & msg);
    virtual bool OnTimer(msgObject & msg);

    void AutoSetKillLongPressTimer(int iKeyStatus);

    bool SetExpInsertDelayTimer(DEVICE_TYPE_DEF expType);
    void KillExpInsertDelayTimer();
    bool OnExpInsertDelayTimer(UINT iTimerId);

    bool SetKeyLongPressTimer();
    void KillKeyLongPressTimer();
    bool OnKeyLongPressTimer(UINT iTimerId);

    bool IsKeyReleaseAfterLongPress(int iKeyStatus);

    DsskeyID AddPageInfoForDsskeyID(DsskeyID dssId);

	int GetCurrentExpPage(ExpID expId);
	void SetCurrentExpPage(ExpID expId, int iPage);

	void SetTestMode(bool bEnable);

	void ExpCountsDecrease();

protected:

	// 用来记录按下的键，用来检查是否同一个键按下抬起
	void RegisterKeyDown(DsskeyID dssId, int iKeyStaus);

    inline DsskeyID GetDsskeyIdByData(int nKeyCode, int nExpIndex, int nDeviceType)
    {
        return ((nDeviceType << 24) + (nExpIndex << 6) + nKeyCode);
    }

    inline int GetKeyCodeFromDsskeyID(DsskeyID dssId)
    {
        return (dssId & 0x3f);
    }

    inline ExpID GetExpIndexFromDssKeyID(DsskeyID dssId)
    {
        return ((dssId >> 6) & 0xf);
    }

    inline DEVICE_TYPE_DEF GetExpTypeFromDssKeyID(DsskeyID dssId)
    {
        return static_cast<DEVICE_TYPE_DEF>(dssId >> 24);
    }

    DEV_MESSAGE_CODE GetMsgFromKeyStatus(int iKeyStaus)
    {
        return iKeyStaus == EXP_KEY_STATUS_PRESS ? DEV_MSG_EXP_KEYDOWN : DEV_MSG_EXP_KEYUP;
    }

    bool IsSameKeyUp(DsskeyID dssId, int iKeyStaus)
    {
        return (iKeyStaus == EXP_KEY_STATUS_RELEASE) ? (m_DssId == dssId) : true;
    }

    inline void RecordInsertExpId(ExpID expId)
    {
        m_InsertId = expId;
    }

public:

    virtual DEVICE_TYPE_DEF  GetMsgProcessType() const = 0;

    inline bool IsTestMode()
    {
        return m_bTestMode;
    }

    inline int GetExpCounts()
    {
        return m_iExpCounts;
    }

    inline DEVICE_TYPE_DEF GetExpTypeFromTimerID(UINT uTimerId)
    {
        return m_MsgProcessType;//(static_cast<DEVICE_TYPE_DEF>(*(UINT *)uTimerId));
    }

    inline void ExpCountsIncrease()
    {
        m_iExpCounts++;
    }

private:

    DEVICE_TYPE_DEF m_MsgProcessType;

protected:

    UINT m_uInsertDelayTimeId;
    bool m_bIsInsertDelayTimerRunning;

    UINT m_uKeyLongPressTimerId;
    bool m_bIsLongPressTimerRunning;

    // 长按之后的弹起标记
    bool m_bKeyReleaseAfterLongPress;

    ExpID m_InsertId;

    DsskeyID m_DssId;

    bool m_bTestMode;

    int m_iExpCounts;

    int m_nCurrentPage[MAX_EXP_NUM];

};
#endif
#endif // !_BASE_EXP_MSG_PROCESS_H__
