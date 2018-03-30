#ifndef _CDLGCALLCONTROLLINESELECT_H_
#define _CDLGCALLCONTROLLINESELECT_H_

#include <ylhashmap.h>
#include "cdlgbasesubpage.h"

class CDlgCallControlLineSelect : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgCallControlLineSelect(QWidget * parent = 0);
    ~CDlgCallControlLineSelect();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgCallControlLineSelect();
    }


public:
    virtual bool LoadPageData();

    virtual bool IsStatusChanged();

    // 从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    virtual void InitData();

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    virtual void AfterSetSubPage(void * pData);
private:
    virtual bool OnAction(const QString & strAction);

private:
    // 下个界面的Action
    yl::string m_strNextPageAction;

    // <账号ID, 项数>
    YLHashMap<int, int> m_mapAssist;

    int                 m_iUIType;
};

#endif // _CDLGCALLCONTROLLINESELECT_H_
