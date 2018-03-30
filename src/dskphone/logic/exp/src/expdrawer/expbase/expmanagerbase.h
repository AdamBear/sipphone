#ifndef _EXP_MANAGER_BASE_H
#define _EXP_MANAGER_BASE_H

#include <dskthread/mkthread.h>
#include "expdefine.h"
#include "myclasses.h"

#define EXP_LIST_FLUSH_MAX_NUM              5
#define EXP_LIST_FLUSH_DSSKEY_MAX_NUM       20      //EXPdsskey按键个数
#define EXP_LIST_FLUSH_KEY_MAX_NUM          23      //EXP总按键个数

class CMyMutex;

class EXPManagerBase
{

public:

    EXPManagerBase();

    virtual ~EXPManagerBase();

public:

    virtual void UpdateList() = 0;

    virtual void Init();

    virtual void Uninit();

    bool GetExpInited()
    {
        return m_bInit;
    }

public:

    LRESULT OnEXPMessage(msgObject & objMessage);

protected:

    bool SetDelayTimer();

    bool OnDelayTimer(UINT uTimerId);


protected:

    virtual void SaveToList(ExpDisplayItemInfo * objUpdateInfo, int nCount);

protected:

    void SetDelayTimeNum(UINT uTimeNum);

    virtual void UpdateMsgDispose(vecExpItemInfo & vecExpInfoFlush, int & nExpNum);

    //合并两个刷新信息
    virtual void ExpDisplayInfoMerge(ExpDisplayItemInfo & objDstData,
                                     ExpDisplayItemInfo & objMergeData);

protected:

    //该数组用以存储刷新的信息，第一维数值为EXP的索引，第二维数值为EXP对应的按键值
    //数组的值为消息中（m_vecExpInfoFlush）对应的刷新信息索引
    bool m_arrbWithData[EXP_LIST_FLUSH_MAX_NUM];

    int m_arrFlushInfo[EXP_LIST_FLUSH_MAX_NUM][EXP_LIST_FLUSH_KEY_MAX_NUM + 1];

protected:

    vecExpItemInfo  m_vecExpInfoReceive;

    CMyMutex        m_mutexListReceive;

    bool            m_arrRefreshBGFlush[EXP_MAX_COUNT];

    bool            m_bInit;

    UINT            m_uInitDelayTime;   // 初始化延迟时间

private:

    /**********************************************************************************************//**
     * @fn  void EXPManagerBase::PostToUpdateList();
     *
     * @brief   Post msg to update list thread to tell it to draw exp.
     **************************************************************************************************/

    void PostToUpdateList();

};

//static CMyMutex s_UpdateMutexed;
//static bool s_bThreadStart;
//static bool s_bUpdating;
//
//IMPLEMENT_THREAD_METHOD(expPaintThread);

#endif //_EXP_MANAGER_H
