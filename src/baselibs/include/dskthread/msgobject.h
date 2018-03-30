#ifndef __MSG_OBJECT_H__
#define __MSG_OBJECT_H__
#include "ETLLib/ETL.h"
typedef unsigned long TARGET;
class msgObject
{
public:
    LPVOID GetExtraData();
    int GetExtraSize();
    void ResetExtraData(int nExtraSize, LPCVOID pExtraData);
    void ReplyMessage(LRESULT result);
    void ReplyMessageEx(LRESULT result, BOOL bAwakeSrcTask);
    void ReplyMessageEx(LRESULT result, int nMsgSize, void * pExtMsg);
    // reset extra data for msgkit test
    void SetMKExtraData(int nExtraSize, const void * pExtraData);
    void SetTarget(TARGET tar);
protected:
    void set_pExtraData(int nExtraSize, LPVOID pData);
public:
    msgObject(UINT uMsgID = 0, WPARAM wPar = 0, LPARAM lPar = 0);
    ~msgObject();
private:
    msgObject(const msgObject & objMessage);
    bool operator == (const msgObject & objMessage);
    msgObject & operator =(const msgObject & objMessage);
public:
    BOOL                m_bIsWaitingReply;
    //eMsgStatue          m_eProcessSta;
    POINTER             m_offsetExtraData;
    int                 m_sizeExtraData;

    void        *       m_pMKExtraData;
public:
    LRESULT             lResult;
    UINT                message;
    WPARAM              wParam;
    LPARAM              lParam;
    TARGET              target;
public:
    int                 m_iPriority;                // lower means hight priority
    TASK_ID             m_idSrcTask;
};

#endif //__MSG_OBJECT_H__
