#ifndef __COMMON_MESSAGEBOX_H__
#define __COMMON_MESSAGEBOX_H__

#include "messageboxbase.h"


class CCommonMessageBox : public CMessageBoxBase
{
public:
    CCommonMessageBox();
    ~CCommonMessageBox();

    static CMessageBoxBase * CreateCommonMsgBox(const MSGBOX_INFO & stMsgInfo);

public:
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    virtual void SetCountMessage(const yl::string & strMessage);
    virtual void RestartExitTime();
    void OnCountTime(xThreadTimer* timer, LPVOID pExtraData);
    void SetCountTime(bool bCountTime);
    void SetShowMessage(int nCountTime);

protected:
    void SetDefaultSoftKey(int nType);

protected:
    VecMsgBoxSoftkey     m_vecSoftkey;
    yl::string m_strMessage;
    int m_nCountTime;
    bool m_bCountTime;
};

#endif // __COMMON_MESSAGEBOX_H__
