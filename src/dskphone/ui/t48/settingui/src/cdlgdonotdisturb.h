#ifndef CDLGDONOTDISTURB_H
#define CDLGDONOTDISTURB_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

class qCheckBox;
struct DndData;

class CDlgDoNotDisturb : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgDoNotDisturb(QWidget * parent = 0);
    ~CDlgDoNotDisturb();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgDoNotDisturb();
    }

private:
    virtual bool GetPageData(DndData * pData);
    virtual bool LoadPageData();
    virtual bool IsStatusChanged();
    virtual bool SavePageData();
    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

private:
    QLineEdit   *   m_pEditOnCode;
    QLineEdit   *   m_pEditOffCode;
    qCheckBox   *   m_qCheckBox;
    int             m_iAccountID;
};

#endif // CDLGDONOTDISTURB_H
