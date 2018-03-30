#ifndef __DLG_CHECK_KEY_GUARD_LOCK_H__
#define __DLG_CHECK_KEY_GUARD_LOCK_H__

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"

class CDlgCheckKeyGuardLock : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgCheckKeyGuardLock();
    }

    CDlgCheckKeyGuardLock(QWidget * parent = 0);
    virtual ~CDlgCheckKeyGuardLock();

    virtual bool IsStatusChanged();
    virtual bool LoadPageData();
    virtual bool SavePageData()
    {
        return true;
    }

    virtual bool OnAction(const QString & strAction);

private:
    void InitData();
    void CheckPassword();

private:
    QLineEdit     *     m_pEditPassword;
};

#endif // #ifndef __DLG_CHECK_KEY_GUARD_LOCK_H__
