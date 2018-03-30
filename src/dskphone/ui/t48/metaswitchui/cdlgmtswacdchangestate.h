#ifndef CDLGMTSWACDCHANGESTATE_H
#define CDLGMTSWACDCHANGESTATE_H

#include "config.h"
#if IF_FEATURE_METASWITCH_ACD

#include "setting/include/common.h"
#ifdef _T48
#include "settingui/src/cdlgbasesubpage.h"
#else
#include "baseui/cdlgbasesubpage.h"
#endif
#include "broadsoft/callcenter/include/cccommon.h"

#include <QtGui/QComboBox>

class CDlgMTSWAcdChangeState : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgMTSWAcdChangeState(QWidget * parent = 0);
    ~CDlgMTSWAcdChangeState();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgMTSWAcdChangeState();
    }


public:
    virtual bool LoadPageData();

    virtual bool IsStatusChanged(); // 查看页面状态是否改变

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    void InitData();

    // 设置页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    // 设置子页面不可编辑时的SoftKey
    virtual bool GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

public Q_SLOTS:

    void OnShow();

protected:
    virtual void showEvent(QShowEvent * e);

private:
    virtual bool OnAction(const QString & strAction);

private:
    std::vector<yl::string> m_vecOrigName;

};
#endif
#endif // CDLGMTSWACDCHANGESTATE_H
