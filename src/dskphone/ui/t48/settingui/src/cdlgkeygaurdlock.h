#ifndef DLG_KEY_GAURD_LOCK_H
#define DLG_KEY_GAURD_LOCK_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

class CDlgKeyGuardLock : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgKeyGuardLock(QWidget * parent = 0);
    ~CDlgKeyGuardLock();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgKeyGuardLock();
    }

public:
    //加载数据
    virtual bool LoadPageData();

    //获取数据状态是否改变
    virtual bool IsStatusChanged();

    //保存数据
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

private:
    //界面初始化
    virtual void InitData();

public:
    QComboBox * m_pCmbKeyGuardEnable;
    QComboBox * m_pCmbKeyGuardType;

    QLineEdit   *   m_pEditKeyGuardTime;
};

#endif // DLGANONYMOUSCALL_H
