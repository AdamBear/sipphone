#ifndef _EXP_MANAGER_H
#define _EXP_MANAGER_H

#include "exppainter.h"
#include "exprefresher.h"

#include <dskthread/mkthread.h>

#include "expshowstatus.h"
#include "expmanagerbase.h"

class EXPManager : public EXPManagerBase, public CExpShowStatusContext
{

public:
    EXPManager();
    virtual ~EXPManager();

public:
    virtual void Init();
    virtual void Uninit();

public:
    LRESULT OnEXPMessage(msgObject & objMessage);

    bool GetExpInited()
    {
        return m_bInit;
    }

    int GetExpCount()
    {
        return m_nExpCount;
    }

protected:
    virtual void OnUpdateBGData(ExpBgData pBGData, int nEXPCount);

    virtual void OnRefreshBG(int nEXPCount, int nEXPIndex);

    virtual void OnUpdateLight(int nEXPCount, int nEXPIndex, int nLightLevel);

    virtual void OnMmapInfo(int nFileHandle, char * pFBMmap, unsigned int nFBMmapLength);

public:
    virtual void UpdateList();

protected:
    virtual void InitMmap();

    virtual void UpdateDsskeyData(const ExpDisplayItemInfo & objUpdateData, bool bFlushAtOnce = true,
                                  bool bFlushLabel = true, bool bFlushLight = true);

    virtual void ResetAllLight(int nEXPCount, int nIndex);

    virtual void UpdateEditingText(char * pStrLabel);

public:
    virtual void RefreshMemPage(int nIndex, int nMemPage);

    virtual bool GetFlushedByIndex(int nIndex);

protected:
    virtual void EXPFillWithColor(bool bUseLocal, EXP_FILL_COLOR objExpColor, int nIndex = 0,
                                  int iCanvasIndex = 1, int iFlushRightNow = 1);
    virtual void EXPFillWithPicture(bool bUseLocal, const char * pStrPicture, int nIndex = 0,
                                    int iCanvasIndex = 1, int iFlushRightNow = 1);
    virtual void EXPDrawText(const char * pStrText, int nIndex = 0);
    virtual void EXPShowLogo(int nIndex = 0);

    virtual void OnExpCountChange(int nNewCount);

    virtual void EnterScreenSaver(bool bEnter);

    bool IsFullPageNeedRefresh(int nExpIndex) const;

private:
    EXPPainter          m_expPainter;
    EXPRefresher        m_expRefresher;

private:
    yl::string              m_strEditingText;

    int                     m_nDownDsskeyID;

    int                     m_nLastIndex;
    int                     m_nSameIndexCount;

    CMyMutex            m_mutexUpdate;

private:

    vecExpItemInfo  m_vecExpInfoFlush;

    CMyMutex            m_mutexListFlush;

private:

    bool                        m_bIsNeedFlushBG;
    ExpBgData                   m_pBgData;

    int                     m_nExpCount;
    int                     m_nExpFirstFlush[EXP_MAX_COUNT];
};

// 获取全局唯一的窗口管理对象
#define _EXPManager GetEXPManagerInstance()
EXPManager & GetEXPManagerInstance();

#endif //_EXP_MANAGER_H
