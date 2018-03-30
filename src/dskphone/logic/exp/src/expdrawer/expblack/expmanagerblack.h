#ifndef _EXP_MANAGER_BLACK_H
#define _EXP_MANAGER_BLACK_H

#include <dskthread/mkthread.h>
#include "../expbase/expmanagerbase.h"

#include "exppainterblack.h"

class EXPManagerBase;

class EXPManagerBlack : public EXPManagerBase
{

public:
    EXPManagerBlack();
    virtual ~EXPManagerBlack();

public:
    virtual void Init();
    virtual void Uninit();

public:
    LRESULT OnEXPMessage(msgObject & objMessage);

    int GetExpCount()
    {
        return m_nExpCount;
    }

protected:

    virtual void OnUpdateLight(int nEXPIndex, int nLightLevel);

    void OnMmapInfo(int nFileHandle);

    void OnUpdateContrast(int nEXPIndex, int nContrastLevel);

    void OnSwitchPage(int nEXPIndex, int nPage);

public:

    virtual void UpdateList();

protected:
    virtual void InitMmap();

    virtual void UpdateDsskeyData(const ExpDisplayItemInfo & objUpdateData);

public:

    virtual bool GetFlushedByIndex(int nIndex);

protected:

    virtual void OnExpCountChange(int nNewCount);

private:

    EXPPainterBlack         m_expPainter;

private:

    CMyMutex                m_mutexUpdate;

private:

    vecExpItemInfo          m_vecExpInfoFlush;
    CMyMutex                m_mutexListFlush;

private:

    int                     m_nExpCount;
    int                     m_nExpFirstFlush[EXP_MAX_COUNT];
};

// 获取全局唯一的窗口管理对象
#define _EXPManager GetEXPManagerInstance()
EXPManagerBlack & GetEXPManagerInstance();

#endif //_EXP_MANAGER_H
