#ifndef DLGSTATUSACCOUNTS_H
#define DLGSTATUSACCOUNTS_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"

class CDlgStatusAccounts : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgStatusAccounts(QWidget * parent = 0);
    ~CDlgStatusAccounts();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgStatusAccounts();
    }

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    static LRESULT StatusAccountMsg(msgObject & objMessage);

public:
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();
    //界面初始化
    void InitPageData();
    //状态显示softkey没有保存项
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftData);

    void OnEnterAction();

    virtual bool OnAction(const QString & strAction);

    //重新显示出来的时候的操作
    virtual void OnReShow();

private:
    bool    m_bEnteredFromStatus;//是否从Status界面进入(默认为true)
    // 该页面从menu进入会刷新两次,导速度问题
    bool m_bFirtEnter;
};

#endif // DLGSTATUSACCOUNTS_H
