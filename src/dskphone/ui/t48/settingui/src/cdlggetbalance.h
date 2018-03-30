#ifndef _DLG_GET_BALANCE_H_
#define _DLG_GET_BALANCE_H_

#include "config.h"
#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

#ifdef IF_FEATURE_GETBALANCE
//  GetBalance 成功之后的显示页面
class CDlgGetBalance : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgGetBalance(QWidget * parent = 0);
    virtual ~CDlgGetBalance();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgGetBalance();
    }

public:
    //界面初始化
    void InitPageData();
    // 读取页面数据
    virtual bool LoadPageData();
    // 查看页面状态是否改变
    virtual bool IsStatusChanged()
    {
        return false;
    }
    // 保存页面数据
    virtual bool SavePageData()
    {
        return true;
    }
    //状态显示softkey没有保存项
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftData);

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

protected slots:
    // 定时器处理函数
    void OnTimer();
    void StopTimer();
    void StartTimer(int timer = 0);

private:
    QTimer   *  m_TimerAutoQuit;
    QLabel   *  m_pLabelAccount;
    QLabel   *  m_pLabelBalance;
};

#endif

#endif  // _DLG_GET_BALANCE_H_
