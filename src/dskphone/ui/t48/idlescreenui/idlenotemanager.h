#ifndef IDLENOTEMANAGER_H
#define IDLENOTEMANAGER_H

#include <QObject>
#include "popupboxproxyfactory.h"
#include"uicommon/messagebox/messageboxcallback.h"
class CBaseIdlePopupboxProxy;

#define IDLE_POP_NOTE_MESSAGEBOX_ID 1

class CIdlePopboxManager : public QObject, public CMsgBoxCallBackAction
{
    Q_OBJECT

public:
    CIdlePopboxManager();
    ~CIdlePopboxManager();
    //核查是否需要弹窗
    void CheckPopbox();
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox) ;

    virtual bool IsUrgentMsgBox(CMessageBoxBase * pMessageBox);

    //关闭弹窗
    void ClosePopupBox(int nCloseReason);
private:
    // 是否允许弹出
    bool IsCanPopup();

    // 是否已经存在弹出框
    bool IsTipPopup();

    //取得弹窗类型
    int GetCurrentPopupType();

    // 锁住
    void Lock()
    {
        m_bLock = true;
    }

    // 解锁
    void Unlock()
    {
        m_bLock = false;
    }

    // 是否锁住
    bool Islock()
    {
        return m_bLock;
    }
    void ShowPopupBox(int eStatus);
    bool CheckNetWorkFailed(int eStatus);
    bool IsShouldHideVolumebar(int nStatus);

private:
    //当前弹窗类型
    CBaseIdlePopupboxProxy * m_pPopupboxProxy;

    // 是否处于正在弹出弹出框的过程,这个时候不处理刷新,用于防抖小于100ms的调用
    bool                    m_bLock;
};

#define  _IdleNoteManager GetIdleNoteManagerInstance()
CIdlePopboxManager & GetIdleNoteManagerInstance();
#endif // IDLENOTEMANAGER_H

