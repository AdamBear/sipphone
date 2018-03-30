#ifndef DLGAUTOPCODEAUTHORIZED_H
#define DLGAUTOPCODEAUTHORIZED_H

#include "globalmodel.h"

#if IF_BUG_23826

#include "cdlgbasesubpage.h"
class QLineEdit;

//AutoPCode的鉴权界面
class CDlgAutoPCodeAuthorized : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgAutoPCodeAuthorized(QWidget * parent = 0);
    ~CDlgAutoPCodeAuthorized();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgAutoPCodeAuthorized();
    }

public:
    // 读取页面数据
    virtual bool LoadPageData();
    // 查看页面状态是否改变
    virtual bool IsStatusChanged();
    // 保存页面数据
    virtual bool SavePageData();

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

private:
    //初始化数据
    void InitData();

    //对事件的处理
    virtual bool OnAction(const QString & strAction);


private:
    QLineEdit   *   m_pEditName;
    QLineEdit   *   m_pEditPassword;
};

#endif // end IF_BUG_23826

#endif // CDLGAUTOREDIAL_H
