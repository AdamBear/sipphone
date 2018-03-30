#ifndef CDlgMTSWAcdMembers_H
#define CDlgMTSWAcdMembers_H

#include "config.h"
#if IF_FEATURE_METASWITCH_ACD
#include "setting/include/common.h"
#ifdef _T48
#include "settingui/src/cdlgbasesubpage.h"
#else
#include "baseui/cdlgbasesubpage.h"
#endif
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "broadsoft/callcenter/include/cccommon.h"

#include <QtGui/QComboBox>

class CDlgMTSWAcdMembers : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgMTSWAcdMembers(QWidget * parent = 0);
    ~CDlgMTSWAcdMembers();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgMTSWAcdMembers();
    }


public:

    //获取MLHG列表
    void GetMLHGMembers();

    virtual bool LoadPageData();

    void InitData();

    virtual bool IsStatusChanged(); // 查看页面状态是否改变

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 设置页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    // 设置子页面不可编辑时的SoftKey
    virtual bool GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

#if IF_SETTING_WND_PRE_NEXT_BTN
    // setting界面是否显示左右切换图标
    virtual bool ShowPreNextBtn()
    {
        return true;
    }
    // 向左翻页
    virtual void OnBtnPreClick();
    // 向右翻页
    virtual void OnBtnNextClick();
#endif

    // 按左右键翻页
    void OnPaging(bool bNext);

public Q_SLOTS:
    void OnShow();

protected:
    virtual void showEvent(QShowEvent * e);

private:
    virtual bool OnAction(const QString & strAction);
};

#endif
#endif // CDlgMTSWAcdMembers_H
