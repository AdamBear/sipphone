#ifndef CDLGCALLCONTROL_H
#define CDLGCALLCONTROL_H

#include "setting/include/common.h"
#include "account/include/modaccount.h"
#include "cdlgbasesubpage.h"
class qCheckBox;

class CDlgCallControl : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgCallControl(QWidget * parent = 0);
    ~CDlgCallControl();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgCallControl();
    }


public:

    virtual bool LoadPageData();

    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    virtual void InitData();

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    virtual void Uninit();

protected:
    yl::string GetClickActionByItemAction(const QString & strItemAction);

    // 添加项
    void AddItem(const QString & strDisplayText, QWidget * pWidget,
                 const QString & strAction/*, bool isMenu = true*/);

    void AddExecutiveItems();
    //添加账号安全等级列表项
    void AddSecurityClassificationItem();

private:
    virtual bool OnAction(const QString & strAction);

};

#endif // CDLGCALLCONTROL_H
