#ifndef CDLGAUTHORITY_H
#define CDLGAUTHORITY_H

#include "setting/setting_data_define.h"
#include "cdlgbasesubpage.h"
#include "setting/include/common.h"
#include "feature/include/featurecommon.h"

class CDlgAuthority : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgAuthority(QWidget * parent = 0);
    ~CDlgAuthority();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgAuthority();
    }

private:

    //从V层读取当前界面的值
    //bool GetPageData(String *pData);

    //通过C层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过C层获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

private Q_SLOTS:
    //void OnSaveAuthorityData();

private:
    QComboBox * m_pCmbAuthority;
    QLineEdit * m_pEditPassword;
    USER_TYPE m_eAuthority; //记录页面载入时的数据
};

#endif // CDLGAUTHORITY_H
