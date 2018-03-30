#ifndef CDLGINTERCOM_H
#define CDLGINTERCOM_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

class qCheckBox;

class CDlgIntercom : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgIntercom(QWidget * parent = 0);
    ~CDlgIntercom();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgIntercom();
    }

private:

    //从V层读取当前界面的值
    bool GetPageData(IntercomData * pData);

    //通过C层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过C层获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

private:
    qCheckBox * m_qCheckBoxIntercomAllow;
    qCheckBox * m_qCheckBoxIntercomMute;
    qCheckBox * m_qCheckBoxIntercomTone;
    qCheckBox * m_qCheckBoxIntercomBarge;
};

#endif // CDLGINTERCOM_H
